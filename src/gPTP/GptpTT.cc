#include "GptpTT.h"

#include "inet/linklayer/ieee8021as/GptpPacket_m.h"

#include "inet/clock/model/SettableClock.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/clock/ClockUserModuleBase.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/linklayer/ethernet/common/Ethernet.h"
#include "inet/linklayer/ieee8021q/Ieee8021qTagHeader_m.h"
#include "inet/linklayer/ethernet/common/EthernetMacHeader_m.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "inet/physicallayer/wired/ethernet/EthernetPhyHeader_m.h"

using namespace std;
using namespace omnetpp;
using namespace inet;

Define_Module(GptpTT);

simsignal_t GptpTT::rateRatioSignal = cComponent::registerSignal("rateRatio");
simsignal_t GptpTT::sequenceIDSignal = cComponent::registerSignal("sequenceID");
simsignal_t GptpTT::peerDelaySignal = cComponent::registerSignal("peerDelay");

const MacAddress GptpTT::GPTP_MULTICAST_ADDRESS("01:80:C2:00:00:0E");


GptpTT::~GptpTT() {
    cancelAndDeleteClockEvent(selfMsgDelayReq);
    cancelAndDeleteClockEvent(requestMsg);
}


void GptpTT::initialize(int stage)
{
    ClockUserModuleBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        domainNumber = par("domainNumber");
        pDelayReqProcessingTime = par("pDelayReqProcessingTime");
        hash<string> strHash;
        clockIdentity = strHash(getFullPath());
    }
    if (stage == INITSTAGE_LINK_LAYER) {
        interfaceTable.reference(this, "interfaceTableModule", true);

        // Ethernet interface == TSN interface
        const char *ethId = "eth0";
        if (*ethId) {
            auto nic = CHK(interfaceTable->findInterfaceByName(ethId));
            tsnPortId = nic->getInterfaceId();
        }else{
            throw cRuntimeError("Parameter error: Missing tt0 for TRANSLATOR_NODE");
        }

        // Slave Port -> calculate peerdelay ifo connected to TSN devices
        const char *slavePort = par("slavePort");
        if (*slavePort) {
            auto nic = CHK(interfaceTable->findInterfaceByName(slavePort));
            slavePortId = nic->getInterfaceId();
            nic->subscribe(transmissionEndedSignal, this);
            nic->subscribe(receptionEndedSignal, this);
        }else{
            throw cRuntimeError("Parameter error: Missing slave port for TRANSLATOR_NODE");
        }

        // Master Ports -> sends Sync messages
        auto v = check_and_cast<cValueArray *>(par("masterPorts").objectValue())->asStringVector();
        if (v.empty())
            throw cRuntimeError("Parameter error: Missing any master port for TRANSLATOR_NODE");
        for (const auto& p : v) {
            auto nic = CHK(interfaceTable->findInterfaceByName(p.c_str()));
            int portId = nic->getInterfaceId();
            if (portId == slavePortId){
                throw cRuntimeError("Parameter error: the port '%s' specified both master and slave port", p.c_str());
            }
            masterPortIds.insert(portId);
            nic->subscribe(transmissionEndedSignal, this);
            nic->subscribe(receptionEndedSignal, this);
        }

        auto networkInterfaceS = interfaceTable->getInterfaceById(slavePortId);
        if (!networkInterfaceS->matchesMulticastMacAddress(GPTP_MULTICAST_ADDRESS))
            networkInterfaceS->addMulticastMacAddress(GPTP_MULTICAST_ADDRESS);

        for (auto id: masterPortIds) {
            auto networkInterface = interfaceTable->getInterfaceById(id);
            if (!networkInterface->matchesMulticastMacAddress(GPTP_MULTICAST_ADDRESS))
                networkInterface->addMulticastMacAddress(GPTP_MULTICAST_ADDRESS);
        }

        //receivedTimeSync = CLOCKTIME_ZERO;
        peerDelay = 0;
        correctionField = CLOCKTIME_ZERO;
        gmRateRatio = 1.0;
        lastReceivedGptpSyncSequenceId = 0;

        registerProtocol(Protocol::gptp, gate("socketOut"), gate("socketIn"));

        if(slavePortId==tsnPortId)
        {
            requestMsg = new ClockEvent("requestToSendSync", GPTP_REQUEST_TO_SEND_SYNC);
            selfMsgDelayReq = new ClockEvent("selfMsgPdelay", GPTP_SELF_MSG_PDELAY_REQ);
            pdelayInterval = par("pdelayInterval");
            scheduleClockEventAfter(par("pdelayInitialOffset"), selfMsgDelayReq);
        }
            WATCH(peerDelay);
    }

}


void GptpTT::handleSelfMessage(cMessage *msg)
{
    switch(msg->getKind()) {
        case GPTP_SELF_REQ_ANSWER_KIND:
            // masterport:
            sendPdelayResp(check_and_cast<GptpReqAnswerEvent*>(msg));
            delete msg;
            break;

        case GPTP_SELF_MSG_PDELAY_REQ:
            // slaveport:
            sendPdelayReq(); //TODO on slaveports only
            scheduleClockEventAfter(pdelayInterval, selfMsgDelayReq);
            break;

        default:
            break;
            //throw cRuntimeError("Unknown self message (%s)%s, kind=%d", msg->getClassName(), msg->getName(), msg->getKind());
    }
}

void GptpTT::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
    }
    else {
        Packet *packet = check_and_cast<Packet *>(msg);
        auto gptp = packet->peekAtFront<GptpBase>();
        auto gptpMessageType = gptp->getMessageType();
        auto incomingNicId = packet->getTag<InterfaceInd>()->getInterfaceId();
        int incomingDomainNumber = gptp->getDomainNumber();

        if (incomingDomainNumber != domainNumber) {
            EV_ERROR << "Message " << msg->getClassAndFullName() << " arrived with foreign domainNumber " << incomingDomainNumber << ", dropped\n";
            PacketDropDetails details;
            details.setReason(NOT_ADDRESSED_TO_US);
            emit(packetDroppedSignal, packet, &details);
        }
        else if (incomingNicId == slavePortId) {
            switch (gptpMessageType) {
                case GPTPTYPE_SYNC:
                    if(slavePortId==tsnPortId){
                        processSyncTSN(packet, check_and_cast<const GptpSync *>(gptp.get()));
                    }else{
                        processSync5G(packet, check_and_cast<const GptpSync *>(gptp.get()));
                    }
                    break;
                case GPTPTYPE_FOLLOW_UP:
                    if(slavePortId==tsnPortId){
                        processFollowUpTSN(packet, check_and_cast<const GptpFollowUp *>(gptp.get()));
                    }else{
                        processFollowUp5G(packet, check_and_cast<const GptpFollowUp *>(gptp.get()));
                    }
                    break;
                case GPTPTYPE_PDELAY_RESP:
                    processPdelayResp(packet, check_and_cast<const GptpPdelayResp *>(gptp.get()));
                    break;
                case GPTPTYPE_PDELAY_RESP_FOLLOW_UP:
                    processPdelayRespFollowUp(packet, check_and_cast<const GptpPdelayRespFollowUp *>(gptp.get()));
                    break;
                default:
                    throw cRuntimeError("Unknown gPTP packet type: %d", (int)(gptpMessageType));
            }
        }
        else if (masterPortIds.find(incomingNicId) != masterPortIds.end()) {
            // master port
            if(gptpMessageType == GPTPTYPE_PDELAY_REQ) {
                processPdelayReq(packet, check_and_cast<const GptpPdelayReq *>(gptp.get()));
            }
            else {
                throw cRuntimeError("Unaccepted gPTP type: %d", (int)(gptpMessageType));
            }
        }
        else {
            EV_ERROR << "Message " << msg->getClassAndFullName() << " arrived on passive port " << incomingNicId << ", dropped\n";
        }
        delete msg;
    }
}

void GptpTT::sendPdelayReq()
{
    ASSERT(slavePortId != -1);
    auto packet = new Packet("GptpPdelayReq");
    packet->addTag<MacAddressReq>()->setDestAddress(GPTP_MULTICAST_ADDRESS);
    auto gptp = makeShared<GptpPdelayReq>();
    gptp->setDomainNumber(domainNumber);
    gptp->setCorrectionField(CLOCKTIME_ZERO);
    //save and send IDs
    PortIdentity portId;
    portId.clockIdentity = clockIdentity;
    portId.portNumber = slavePortId;
    gptp->setSourcePortIdentity(portId);
    lastSentPdelayReqSequenceId = sequenceIdPeerDelay++;
    gptp->setSequenceId(lastSentPdelayReqSequenceId);
    packet->insertAtFront(gptp);
    rcvdPdelayResp = false;
    sendPacketToNIC(packet, slavePortId);
}

void GptpTT::processPdelayReq(Packet *packet, const GptpPdelayReq* gptp)
{
    auto resp = new GptpReqAnswerEvent("selfMsgPdelayResp", GPTP_SELF_REQ_ANSWER_KIND);
    resp->setPortId(packet->getTag<InterfaceInd>()->getInterfaceId());
    resp->setIngressTimestamp(packet->getTag<GptpIngressTimeInd>()->getArrivalClockTime());
    resp->setSourcePortIdentity(gptp->getSourcePortIdentity());
    resp->setSequenceId(gptp->getSequenceId());
    scheduleClockEventAfter(pDelayReqProcessingTime, resp);
}

void GptpTT::sendPdelayResp(GptpReqAnswerEvent* req)
{
    int portId = req->getPortId();
    auto packet = new Packet("GptpPdelayResp");
    packet->addTag<MacAddressReq>()->setDestAddress(GPTP_MULTICAST_ADDRESS);
    auto gptp = makeShared<GptpPdelayResp>();
    gptp->setDomainNumber(domainNumber);
    gptp->setRequestingPortIdentity(req->getSourcePortIdentity());
    gptp->setSequenceId(req->getSequenceId());
    gptp->setRequestReceiptTimestamp(req->getIngressTimestamp()*gmRateRatio);
    packet->insertAtFront(gptp);
    sendPacketToNIC(packet, portId);
}

void GptpTT::sendPdelayRespFollowUp(int portId, const GptpPdelayResp* resp, clocktime_t preciseOriginTimestamp)
{
    auto packet = new Packet("GptpPdelayRespFollowUp");
    packet->addTag<MacAddressReq>()->setDestAddress(GPTP_MULTICAST_ADDRESS);
    auto gptp = makeShared<GptpPdelayRespFollowUp>();
    gptp->setDomainNumber(domainNumber);
    gptp->setResponseOriginTimestamp(preciseOriginTimestamp*gmRateRatio);
    gptp->setRequestingPortIdentity(resp->getRequestingPortIdentity());
    gptp->setSequenceId(resp->getSequenceId());
    packet->insertAtFront(gptp);
    sendPacketToNIC(packet, portId);
}

void GptpTT::processPdelayResp(Packet *packet, const GptpPdelayResp* gptp)
{
    // verify IDs
    if (gptp->getRequestingPortIdentity().clockIdentity != clockIdentity || gptp->getRequestingPortIdentity().portNumber != slavePortId) {
        EV_WARN << "GptpPdelayResp arrived with invalid PortIdentity, dropped";
        return;
    }
    if (gptp->getSequenceId() != lastSentPdelayReqSequenceId) {
        EV_WARN << "GptpPdelayResp arrived with invalid sequence ID, dropped";
        return;
    }

    rcvdPdelayResp = true;
    pdelayRespEventIngressTimestamp = packet->getTag<GptpIngressTimeInd>()->getArrivalClockTime(); // T3
    peerRequestReceiptTimestamp = gptp->getRequestReceiptTimestamp(); // T2
}

void GptpTT::processPdelayRespFollowUp(Packet *packet, const GptpPdelayRespFollowUp* gptp)
{
    if (!rcvdPdelayResp) {
        EV_WARN << "GptpPdelayRespFollowUp arrived without GptpPdelayResp, dropped";
        return;
    }
    // verify IDs
    if (gptp->getRequestingPortIdentity().clockIdentity != clockIdentity || gptp->getRequestingPortIdentity().portNumber != slavePortId) {
        EV_WARN << "GptpPdelayRespFollowUp arrived with invalid PortIdentity, dropped";
        return;
    }
    if (gptp->getSequenceId() != lastSentPdelayReqSequenceId) {
        EV_WARN << "GptpPdelayRespFollowUp arrived with invalid sequence ID, dropped";
        return;
    }

    peerResponseOriginTimestamp = gptp->getResponseOriginTimestamp();

    calculatePeerDelay();
}

void GptpTT::calculatePeerDelay(){
    peerDelay = (gmRateRatio * (pdelayRespEventIngressTimestamp - pdelayReqEventEgressTimestamp) - (peerResponseOriginTimestamp - peerRequestReceiptTimestamp)) / 2.0;

    EV_INFO << "RATE RATIO                       - " << gmRateRatio << endl;
    EV_INFO << "pdelayReqEventEgressTimestamp    - " << pdelayReqEventEgressTimestamp << endl;
    EV_INFO << "peerResponseOriginTimestamp      - " << peerResponseOriginTimestamp << endl;
    EV_INFO << "pdelayRespEventIngressTimestamp  - " << pdelayRespEventIngressTimestamp << endl;
    EV_INFO << "peerRequestReceiptTimestamp      - " << peerRequestReceiptTimestamp << endl;
    EV_INFO << "PEER DELAY   9999                    - " << peerDelay << endl;

    emit(peerDelaySignal, CLOCKTIME_AS_SIMTIME(peerDelay));
}

void GptpTT::processSyncTSN(Packet *packet, const GptpSync* gptp)
{
    auto copyPacket = new Packet("GptpSync");
    copyPacket->addTag<MacAddressReq>()->setDestAddress(GPTP_MULTICAST_ADDRESS);

    lastReceivedGptpSyncSequenceId = gptp->getSequenceId();
    syncIngressTimestamp = packet->getTag<GptpIngressTimeInd>()->getArrivalClockTime();

    auto copyGptp = makeShared<GptpSync>();
    copyGptp->setDomainNumber(domainNumber);
    copyGptp->setSequenceId(lastReceivedGptpSyncSequenceId);

    copyPacket->insertAtFront(copyGptp);
    for (auto port: masterPortIds)
        sendPacketToNIC(copyPacket->dup(), port);

    delete copyPacket;
}

void GptpTT::processSync5G(Packet *packet, const GptpSync* gptp)
{
    //rcvdGptpSync = true;
    //rcvdGptpSyncFollowUp = true;

    auto copyPacket = new Packet("GptpSync");
    copyPacket->addTag<MacAddressReq>()->setDestAddress(GPTP_MULTICAST_ADDRESS);

    int gptpSequenceID = gptp->getSequenceId();
    if(gptpSequenceID < lastReceivedGptpSyncSequenceId){
        EV_WARN << "Old information. Dropping" << endl;
        return;
    }
    lastReceivedGptpSyncSequenceId = gptp->getSequenceId();
    syncIngressTimestamp = packet->getTag<GptpIngressTimeInd>()->getArrivalClockTime();

    auto copyGptp = makeShared<GptpSync>();
    copyGptp->setDomainNumber(domainNumber);
    copyGptp->setSequenceId(lastReceivedGptpSyncSequenceId);

    copyPacket->insertAtFront(copyGptp);
    for (auto port: masterPortIds)
        sendPacketToNIC(copyPacket->dup(), port);

    delete copyPacket;
}

void GptpTT::processFollowUpTSN(Packet *packet, const GptpFollowUp* gptp)
{
    // verify IDs
    int gptpSequenceID = gptp->getSequenceId();
    if(gptpSequenceID < lastReceivedGptpSyncSequenceId){
        EV_WARN << "Old information. Dropping" << endl;
        return;
    }
    if (gptpSequenceID != lastReceivedGptpSyncSequenceId) {
        EV_WARN << "GptpFollowUp arrived with invalid sequence ID, dropped";
        return;
    }
    originTimestamp = gptp->getPreciseOriginTimestamp();
    peerSentTimeSync = gptp->getPreciseOriginTimestamp() + gptp->getCorrectionField();
    receivedRateRatio = gptp->getFollowUpInformationTLV().getRateRatio();
    correctionField = gptp->getCorrectionField();

    rcvdGptpSyncFollowUp = true;
    if(rcvdGptpSync){
        sendFollowUp(slavePortId, sentTimeSyncSync);
    }

}

void GptpTT::processFollowUp5G(Packet *packet, const GptpFollowUp* gptp)
{
    // verify IDs
    int gptpSequenceID = gptp->getSequenceId();
    if(gptpSequenceID < lastReceivedGptpSyncSequenceId){
        EV_WARN << "Old information. Dropping" << endl;
        return;
    }
    if (gptpSequenceID != lastReceivedGptpSyncSequenceId) {
        EV_WARN << "GptpFollowUp arrived with out of order. Waiting for syncMessage" << endl;
    }
    originTimestamp = gptp->getPreciseOriginTimestamp();
    peerSentTimeSync = gptp->getPreciseOriginTimestamp() + gptp->getCorrectionField();
    receivedRateRatio = gptp->getFollowUpInformationTLV().getRateRatio();
    correctionField = gptp->getCorrectionField();
    suffValue = gptp->getSuff();

    rcvdGptpSyncFollowUp = true;
    if(rcvdGptpSync){
        sendFollowUp(slavePortId, sentTimeSyncSync);
    }

}

void GptpTT::sendFollowUp(int portId, clocktime_t syncTxEndTimestamp)
{

    auto packet = new Packet("GptpFollowUp");
    packet->addTag<MacAddressReq>()->setDestAddress(GPTP_MULTICAST_ADDRESS);
    auto gptp = makeShared<GptpFollowUp>();
    gptp->setDomainNumber(domainNumber);

    gptp->setSequenceId(lastReceivedGptpSyncSequenceId);

    clocktime_t CR;
    double rateRatio;

    EV << "Enviando FollowUp" << endl;
    EV << "OriginTimestamp" << originTimestamp << endl;
    EV << "CorrectionField received: " << correctionField << endl;
    if (portId == tsnPortId){
        if (oldPeerSentTimeSync == -1)
            gmRateRatio = 1;
        else{
//            gmRateRatio = (syncIngressTimestamp - receivedTimeSync)/(originTimestamp - oldPeerSentTimeSync) ;
            //gmRateRatio = (syncIngressTimestamp - receivedTimeSync)/(peerSentTimeSync - oldPeerSentTimeSync);
            gmRateRatio = (peerSentTimeSync - oldPeerSentTimeSync)/(syncIngressTimestamp - receivedTimeSync);
            EV_INFO << "(DEN) ------- "<< (syncIngressTimestamp - receivedTimeSync) << endl;
            EV_INFO << "(NUM) ------- "<< (peerSentTimeSync - oldPeerSentTimeSync) << endl;
            EV_INFO << "gmRateRatio-1 ------- "<< (syncIngressTimestamp - receivedTimeSync)/(peerSentTimeSync - oldPeerSentTimeSync) << endl;
            EV_INFO << "gmRateRatio+1 ------- "<< (peerSentTimeSync - oldPeerSentTimeSync)/(syncIngressTimestamp - receivedTimeSync) << endl;
        }
        oldPeerSentTimeSync = peerSentTimeSync;
        receivedTimeSync = syncIngressTimestamp;

        //timeDiffAtTimeSync = newLocalTimeAtTimeSync - oldLocalTimeAtTimeSync;
        //receivedTimeSync+=timeDiffAtTimeSync;

        EV << "FollowUp passando pelo 5G" << endl;
        gptp->setPreciseOriginTimestamp(originTimestamp);
        gptp->setSuff(-(syncIngressTimestamp)*(gmRateRatio));
        rateRatio = gmRateRatio;
        CR = correctionField+ peerDelay;
        rateRatio = gmRateRatio;
    }else if (portId != tsnPortId){
        EV << "FollowUp saindo do 5G" << endl;
        gptp->setPreciseOriginTimestamp(originTimestamp);
        rateRatio = receivedRateRatio;
        gmRateRatio = receivedRateRatio;
        CR = correctionField+sentTimeSyncSync*rateRatio+suffValue;
    }
    EV << "syncIngressTimestamp" << syncIngressTimestamp << endl;
    EV << "sentTimeSyncSync" << sentTimeSyncSync << endl;
    EV << "peerDelay" << peerDelay << endl;
    EV << "receivedRateRatio" << receivedRateRatio << endl;
    EV << "gmRateRatio" << gmRateRatio << endl;

    emit(rateRatioSignal, gmRateRatio);
    emit(sequenceIDSignal, int(lastReceivedGptpSyncSequenceId));


    gptp->setCorrectionField(CR);
    gptp->getFollowUpInformationTLVForUpdate().setRateRatio(rateRatio);

    packet->addTagIfAbsent<InterfaceInd>()->setInterfaceId(portId);

    packet->insertAtFront(gptp);
    EV << "correctionField Sent" << CR << endl;
    for (auto port: masterPortIds)
        sendPacketToNIC(packet->dup(), port);

    rcvdGptpSync = false;
    rcvdGptpSyncFollowUp = false;
    delete packet;
}

void GptpTT::sendPacketToNIC(Packet *packet, int portId)
{
    auto networkInterface = interfaceTable->getInterfaceById(portId);
    EV_INFO << "Sending " << packet << " to output interface = " << networkInterface->getInterfaceName() << ".\n";
    packet->addTag<InterfaceReq>()->setInterfaceId(portId);
    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::gptp);
    packet->addTagIfAbsent<DispatchProtocolInd>()->setProtocol(&Protocol::gptp);
    auto protocol = networkInterface->getProtocol();
    if (protocol != nullptr)
        packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(protocol);
    else
        packet->removeTagIfPresent<DispatchProtocolReq>();
    send(packet, "socketOut");
}


void GptpTT::receiveSignal(cComponent *source, simsignal_t signal, cObject *obj, cObject *details)
{
    Enter_Method("%s", cComponent::getSignalName(signal));

    if (signal == receptionEndedSignal) {
        auto signal = check_and_cast<cPacket *>(obj);
        auto packet = check_and_cast_nullable<Packet *>(signal->getEncapsulatedPacket());
        if (packet) {
            auto protocol = packet->getTag<PacketProtocolTag>()->getProtocol();
            EV << "Pacote chegou" << endl;
            if (*protocol == Protocol::ethernetPhy) {
                const auto& ethPhyHeader = packet->peekAtFront<physicallayer::EthernetPhyHeader>();
                const auto& ethMacHeader = packet->peekAt<EthernetMacHeader>(ethPhyHeader->getChunkLength());
                EV << "Tem header ethPrhy e ethMac" << endl;
                if (ethMacHeader->getTypeOrLength() == ETHERTYPE_GPTP) {
                    const auto& gptp = packet->peekAt<GptpBase>(ethPhyHeader->getChunkLength() + ethMacHeader->getChunkLength());
                    if (gptp->getDomainNumber() == domainNumber)
                        packet->addTagIfAbsent<GptpIngressTimeInd>()->setArrivalClockTime(clock->getClockTime());
                }
                else if (ethMacHeader->getTypeOrLength() != ETHERTYPE_IPv4){
                    if(ethMacHeader->getTypeOrLength() != ETHERTYPE_8021Q_TAG){
                        EV << "Trocar, é esse" << endl;
                    }
                    const auto& ethVLANHeader = packet->peekAt<Ieee8021qTagEpdHeader>(ethPhyHeader->getChunkLength()+ethMacHeader->getChunkLength());
                    if (ethVLANHeader->getTypeOrLength() == ETHERTYPE_GPTP) {
                        const auto& gptp = packet->peekAt<GptpBase>(ethPhyHeader->getChunkLength() + ethMacHeader->getChunkLength() + ethVLANHeader->getChunkLength());
                        if (gptp->getDomainNumber() == domainNumber)
                            packet->addTagIfAbsent<GptpIngressTimeInd>()->setArrivalClockTime(clock->getClockTime());
                    }
                }
            }
        }
    }



    else if (signal == transmissionEndedSignal) {
        auto signal = check_and_cast<cPacket *>(obj);
        auto packet = check_and_cast_nullable<Packet *>(signal->getEncapsulatedPacket());
        if (packet) {
            auto protocol = packet->getTag<PacketProtocolTag>()->getProtocol();
            if (*protocol == Protocol::ethernetPhy) {
                const auto& ethPhyHeader = packet->peekAtFront<physicallayer::EthernetPhyHeader>();
                const auto& ethMacHeader = packet->peekAt<EthernetMacHeader>(ethPhyHeader->getChunkLength());
                if (ethMacHeader->getTypeOrLength() == ETHERTYPE_GPTP) {
                    const auto& gptp = packet->peekAt<GptpBase>(ethPhyHeader->getChunkLength() + ethMacHeader->getChunkLength());
                    if (gptp->getDomainNumber() == domainNumber) {
                        int portId = getContainingNicModule(check_and_cast<cModule*>(source))->getInterfaceId();
                        switch (gptp->getMessageType()) {
                            case GPTPTYPE_PDELAY_RESP: {
                                auto gptpResp = dynamicPtrCast<const GptpPdelayResp>(gptp);
                                sendPdelayRespFollowUp(portId, gptpResp.get(),clock->getClockTime());
                                break;
                            }
                            case GPTPTYPE_SYNC: {
                                auto gptpSync = dynamicPtrCast<const GptpSync>(gptp);
                                sentTimeSyncSync = clock->getClockTime();
                                EV << "sentTimeSyncSync: " << sentTimeSyncSync << endl;
                                rcvdGptpSync = true;
                                if(rcvdGptpSyncFollowUp){
                                    sendFollowUp(slavePortId, clock->getClockTime());
                                }
                                break;
                            }
                            case GPTPTYPE_PDELAY_REQ:
                                if (portId == slavePortId)
                                    pdelayReqEventEgressTimestamp = clock->getClockTime();
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
    }

}


