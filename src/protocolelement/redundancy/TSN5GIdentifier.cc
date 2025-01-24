//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "TSN5GIdentifier.h"

#include "inet/common/packet/PacketFilter.h"
#include "inet/common/SequenceNumberTag_m.h"
#include "inet/protocolelement/redundancy/StreamTag_m.h"
#include "inet/linklayer/ieee8021q/Ieee8021qTagHeader_m.h"
#include "src/linklayer/common/QfiTag_m.h"

using namespace inet;

Define_Module(TSN5GIdentifier);

void TSN5GIdentifier::initialize(int stage)
{
    PacketFlowBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        hasSequenceNumbering = par("hasSequenceNumbering");
        qosMap = check_and_cast<QoSMap*>(getModuleByPath("^.^.^.^.qosMap"));
        //mapping = check_and_cast<cValueArray *>(par("mapping").objectValue());
    }
    if (stage == INITSTAGE_LINK_LAYER) {
        interfaceTable.reference(getParentModule()->getParentModule(), "interfaceTableModule", true);
        iTSNInterface = CHK(interfaceTable->findInterfaceByName("eth0"))->getInterfaceId();
        i5GInterface = CHK(interfaceTable->findInterfaceByName("tt0"))->getInterfaceId();
        EV << "iTSNInterface: " << iTSNInterface << endl;
        EV << "i5GInterface: " << i5GInterface << endl;
    }
}

void TSN5GIdentifier::handleParameterChange(const char *name)
{
    //if (!strcmp(name, "mapping"))
        //mapping = check_and_cast<cValueArray *>(par("mapping").objectValue());
}

cGate *TSN5GIdentifier::getRegistrationForwardingGate(cGate *gate)
{
    if (gate == outputGate)
        return inputGate;
    else if (gate == inputGate)
        return outputGate;
    else
        throw cRuntimeError("Unknown gate");
}

int TSN5GIdentifier::incrementSequenceNumber(const char *stream)
{
    auto it = sequenceNumbers.find(stream);
    if (it == sequenceNumbers.end())
        it = sequenceNumbers.insert({stream, 0}).first;
    return it->second++;
}

void TSN5GIdentifier::processPacket(Packet *packet)
{
    //const char *streamName = nullptr;
    //const auto& pcpInd = packet->findTag<PcpInd>();
    const auto& qfiInd = packet->findTag<QfiInd>();
    //const auto& interfaceInd = packet->findTag<InterfaceInd>();
    if (qfiInd != nullptr){
        EV << "qfi tag: " << endl;
    }

    /*
    auto interfaceReq = packet->getTag<InterfaceInd>();
    packet->removeTagIfPresent<StreamReq>();
    if(interfaceReq->getInterfaceId() == iTSNInterface){
        packet->addTag<StreamReq>()->setStreamName("FiveGFlow");
    }else if(interfaceReq->getInterfaceId() == i5GInterface){
        packet->addTag<StreamReq>()->setStreamName("TSNFlow");
    }else{

    }
    //packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(interfaceReq->getInterfaceId());

    auto streamReq = packet->findTag<StreamReq>();
    if (streamReq != nullptr)
        streamName = streamReq->getStreamName();
    else {
        for (int i = 0; i < mapping->size(); i++) {
            packet->getTag<InterfaceReq>()
            auto element = check_and_cast<cValueMap *>(mapping->get(i).objectValue());
            PacketFilter packetFilter;
            auto packetPattern = element->containsKey("packetFilter") ? element->get("packetFilter") : cValue("*");
            packetFilter.setExpression(packetPattern);
            if (packetFilter.matches(packet)) {
                streamName = element->get("stream").stringValue();
                packet->addTag<StreamReq>()->setStreamName(streamName);
                auto sequenceNumber = incrementSequenceNumber(streamName);
                if (hasSequenceNumbering && element->containsKey("sequenceNumbering") && element->get("sequenceNumbering").boolValue())
                    packet->addTag<SequenceNumberReq>()->setSequenceNumber(sequenceNumber);
                break;
            }
        }

    }
    */
    handlePacketProcessed(packet);
    updateDisplayString();
}
