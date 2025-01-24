//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "FGEncoder.h"

#include "inet/common/ProtocolUtils.h"
#include "inet/common/SequenceNumberTag_m.h"
#include "inet/linklayer/common/PcpTag_m.h"
#include "src/linklayer/common/QfiTag_m.h"
#include "inet/protocolelement/redundancy/StreamTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"


using namespace inet;

Define_Module(FGEncoder);

void FGEncoder::initialize(int stage)
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


cGate *FGEncoder::getRegistrationForwardingGate(cGate *gate)
{
    if (gate == outputGate)
        return inputGate;
    else if (gate == inputGate)
        return outputGate;
    else
        throw cRuntimeError("Unknown gate");
}

void FGEncoder::processPacket(Packet *packet)
{
    //const auto& qfiInd = packet->findTag<QfiInd>();
    const auto& interface = packet->findTag<InterfaceInd>();
    if (interface != nullptr){

        int intID = interface->getInterfaceId();
        EV << "Encoder:: Indice da interface: " << intID << endl;
        if(intID==iTSNInterface){
            EV << "Interface do TSN" << endl;
            const auto& pcpInd = packet->findTag<PcpReq>();
            if (pcpInd == nullptr){
                //packet->addTag<QfiReq>()->setQfi(87);
                EV << "Encoder não achou PcpReq" << endl;
                packet->removeTagIfPresent<PcpReq>();
                //packet->addTag<PcpReq>()->setPcp(0);
                //packet->addTag<QfiReq>()->setQfi(qosMap->getPcpFromQfi(3));
            }else{
                int pcp = pcpInd->getPcp();

                EV << "Encoder achou PcpReq: PCP=" << pcp << " QFI:" << qosMap->getQfiFromPcp(pcp) << endl;
                packet->addTag<QfiReq>()->setQfi(qosMap->getQfiFromPcp(pcp));
            }
            //const auto& qfiInd = packet->findTag<QfiReq>();

        }/*
        else if(i5GInterface.find(iIReq) != i5GInterface.end()){
            EV << "Interface do 5G" << endl;
            const auto& pcpReq = packet->findTag<PcpReq>();
            if (pcpReq != nullptr){
                EV << "Achou pcpReq" << endl;
                int pcp = pcpReq->getPcp();
                packet->removeTagIfPresent<PcpInd>();

                packet->removeTagIfPresent<QfiReq>();
                packet->addTag<QfiReq>()->setQfi(qosMap->getQfiFromPcp(pcp));
            }else{
                EV << "N achou pcpReq" << endl;
                packet->removeTagIfPresent<QfiReq>();
                packet->addTag<QfiReq>()->setQfi(1);
            }
        }*/
    }
}

