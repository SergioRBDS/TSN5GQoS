//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package src.delayer;

import inet.protocolelement.processing.IProcessingDelayLayer;
import inet.protocolelement.processing.ProcessingDelayLayer;

// This module is to be used as the delayLayer within a TsnTranslator.
//
// The delayDownlink parameter is used to specify the delay in the downlink direction (from the network to the UE).
// This means, we delay every frame at the earliest possible point in the TsnTranslator
// (directly after arriving from another TsnTranslator), which is at the ingress direction of the parent bridging layer.
// We know that all frames arriving from another TsnTranslator (i.e. a tt interface) are downlink frames.
// Therefore, we set the ingress interface type to "tt" and the delay to delayDownlink.
//
// The delayUplink parameter is used to specify the delay in the uplink direction (from the UE to the network).
// This means, we delay every frame at the latest possible point in the TsnTranslator
// (directly before leaving to another TsnTranslator), which is at the egress direction of the parent bridging layer.
// We know that all frames coming from the UE (i.e. an eth interface) and leaving to another TsnTranslator
// (i.e. a tt interface) are uplink frames.
// Therefore, we set the ingress interface type to "eth" and the egress interface type to "tt"
// and the delay to delayUplink.
module FGDelayLayer extends ProcessingDelayLayer like IProcessingDelayLayer
{
    parameters:

        volatile double delay @mutable @unit(s) = default(0s); // Uplink Delay
        volatile object delayQFI @mutable @unit(s) = default({}); // Uplink Delay
        
        //volatile double delayDownlink @mutable @unit(s) = default(0s); // Downlink Delay

		ingress.typename = default("");//default("FGDelayer");
        //ingress.delay = default(this.delayDownlink);

        // Egress for uplink
        // Arrives at eth (ingress) --> bridging logic --> delay --> tt (egress, to other UE or NWTT)
        egress.typename = default("FGDelayer");
        egress.delay = default(this.delay);
        egress.delayQFI = default(this.delayQFI.dup());
}
