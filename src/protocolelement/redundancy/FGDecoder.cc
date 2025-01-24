//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "FGDecoder.h"

#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/linklayer/common/PcpTag_m.h"
#include "src/linklayer/common/QfiTag_m.h"
#include "inet/common/TagBase_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "inet/protocolelement/redundancy/StreamTag_m.h"

using namespace inet;

Define_Module(FGDecoder);

void FGDecoder::initialize(int stage)
{
    PacketFlowBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL){
        interfaceTable.reference(this, "interfaceTableModule", true);
    }
    if (stage == INITSTAGE_LINK_LAYER) {
        iTSNInterface = CHK(interfaceTable->findInterfaceByName("eth0"))->getInterfaceId();
        int numTT = 0;
        for(int i=0;i<interfaceTable->getNumInterfaces();i++){
            if(interfaceTable->getInterface(i)->getInterfaceName() == (std::string("tt")+std::to_string(numTT))){
                auto a = std::string("tt")+std::to_string(numTT);
                auto aux =  CHK(interfaceTable->findInterfaceByName(a.c_str()))->getInterfaceId();
                i5GInterface.insert(aux);
                numTT++;

            }
        }
        //i5GInterface = CHK(interfaceTable->get find findInterfaceByName("tt0"))->getInterfaceId();
        EV << "iTSNInterface: " << iTSNInterface << endl;
        for (const int& value : i5GInterface) {
            EV << "i5GInterface: " << value << endl;
        }
        qosMap = check_and_cast<QoSMap*>(getModuleByPath("^.^.^.^.qosMap"));
        qosMap->printMap();
    }
}



cGate *FGDecoder::getRegistrationForwardingGate(cGate *gate)
{
    if (gate == outputGate)
        return inputGate;
    else if (gate == inputGate)
        return outputGate;
    else
        throw cRuntimeError("Unknown gate");
}

void FGDecoder::processPacket(Packet *packet)
{

    const auto& interface = packet->findTag<InterfaceInd>();
    if (interface != nullptr){
        int intID = interface->getInterfaceId();
        EV << "Decode:: Indice da interface: " << intID << endl;
/*
        if(i5GInterface.find(intID) != i5GInterface.end()){
            EV << "Interface do 5G" << endl;
            const auto& qfiReq = packet->findTag<QfiReq>();
            if (qfiReq != nullptr){
                EV << "Achou qfiInd" << endl;
                int qfi = qfiReq->getQfi();
                packet->removeTagIfPresent<QfiReq>();

                packet->removeTagIfPresent<PcpInd>();
                packet->addTag<PcpInd>()->setPcp(qosMap->getPcpFromQfi(qfi));
            }else{
                EV << "Não achou qfiInd" << endl;
                packet->removeTagIfPresent<PcpInd>();
                packet->addTag<PcpInd>()->setPcp(6);
            }
        }/*else if(i5GInterface.find(iIInd) != i5GInterface.end()){
            EV << "Interface do 5G" << endl;
            const auto& qfiInd = packet->findTag<QfiInd>();
            if (qfiInd == nullptr){
                EV << "Decoder não achou qfiInd" << endl;
                packet->removeTagIfPresent<QfiInd>();
                packet->addTag<PcpInd>()->setPcp(0);
            }else{
                packet->addTag<PcpInd>()->setPcp(0);
                EV << "Decoder: PCP adicionado" << endl;
            }
        }*/
    }
    handlePacketProcessed(packet);
    updateDisplayString();
}

