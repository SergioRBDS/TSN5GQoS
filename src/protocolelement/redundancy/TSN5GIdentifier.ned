//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package src.protocolelement.redundancy;

import inet.queueing.base.PacketFlowBase;
import inet.queueing.contract.IPacketFlow;


//
simple TSN5GIdentifier extends PacketFlowBase like IPacketFlow
{
    parameters:
        bool hasSequenceNumbering = default(true);
        object mapping @mutable = default([]); 
        @class(TSN5GIdentifier);
        @display("i=block/blackboard");
}
