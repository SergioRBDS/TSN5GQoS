//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_FGENCODER_H
#define __INET_FGENCODER_H

#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/queueing/base/PacketFlowBase.h"
#include "inet/common/ModuleRefByPar.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "../../devices/QoSMap.h"

using namespace inet;

using namespace inet::queueing;

class FGEncoder : public PacketFlowBase, public TransparentProtocolRegistrationListener
{
  protected:
    ModuleRefByPar<IInterfaceTable> interfaceTable;
    QoSMap* qosMap;

    int iTSNInterface = -1;
    std::set<int> i5GInterface;
  protected:
    virtual void initialize(int stage) override;
    virtual void processPacket(Packet *packet) override;

    virtual cGate *getRegistrationForwardingGate(cGate *gate) override;
};


#endif

