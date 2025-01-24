//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package src.protocolelement.redundancy;

import inet.queueing.base.PacketFlowBase;
import inet.queueing.contract.IPacketFlow;

//
// This module decodes the stream of a packet by matching various tags on the
// packet against the mapping. It can check for network interface, source address,
// destination address, VLAN id, and PCP. If a match was found then the stream
// is attached to the packet in a ~StreamInd tag.
//
// @see ~StreamEncoder
//
simple FGDecoder extends PacketFlowBase like IPacketFlow
{
    parameters:
        string interfaceTableModule; // relative module path of the interface table
        @class(FGDecoder);
        @display("i=block/blackboard");
}
