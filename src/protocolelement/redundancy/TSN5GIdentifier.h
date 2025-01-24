//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_TSN5GIDENTIFIER_H
#define __INET_TSN5GIDENTIFIER_H

#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/queueing/base/PacketFlowBase.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/PcpTag_m.h"

#include "../../devices/QoSMap.h"


using namespace inet;

using namespace inet::queueing;

class TSN5GIdentifier : public PacketFlowBase, public TransparentProtocolRegistrationListener
{
  protected:
    ModuleRefByPar<IInterfaceTable> interfaceTable;
    QoSMap* qosMap;
    bool hasSequenceNumbering = false;
    std::map<std::string, int> sequenceNumbers;

    int iTSNInterface = -1;
    int i5GInterface = -1;
  protected:
    virtual void initialize(int stage) override;
    virtual void handleParameterChange(const char *name) override;
    virtual void processPacket(Packet *packet) override;
    virtual int incrementSequenceNumber(const char *stream);

    virtual cGate *getRegistrationForwardingGate(cGate *gate) override;
};


#endif

