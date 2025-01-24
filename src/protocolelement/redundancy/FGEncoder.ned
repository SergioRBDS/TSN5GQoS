//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package src.protocolelement.redundancy;

import inet.queueing.base.PacketFlowBase;
import inet.queueing.contract.IPacketFlow;

//
// This module encodes the stream name of a packet into several request tags
// attached to the packet. It looks up the stream in the mapping and attaches
// the necessary request tags to the packet based on the specified parameters.
//
// @see ~StreamDecoder
//
simple FGEncoder extends PacketFlowBase like IPacketFlow
{
    parameters:
        string interfaceTableModule; // relative module path of the interface table
        @class(FGEncoder);
        @display("i=block/blackboard");
}
