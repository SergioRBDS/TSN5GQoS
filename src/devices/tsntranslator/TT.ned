package src.devices.tsntranslator;

import inet.node.tsn.TsnSwitch;
import inet.linklayer.contract.IEthernetInterface;

module TT extends TsnSwitch
{
    parameters:
        int numTtInterfaces = default(0);
        bool isDstt = default(false);
        bool schedule = default(false);

        volatile double delayUplink @mutable @unit(s) = default(0s); // Uplink Delay
        volatile double delayDownlink @mutable @unit(s) = default(0s); // Downlink Delay
        volatile double delayD2D @mutable @unit(s) = default(0s); // Downlink Delay
        volatile object delayQFI @mutable @unit(s) = default({}); // Downlink Delay

		gptp.typename = default("MultiDomainGptp");
		gptp.domain[*].typename = default("GptpTT");
		gptp.numDomains = default(1);
		gptp.gptpNodeType = "BRIDGE_NODE";
		gptp.domain[*].clockModule = default(absPath(gptp.clockModule));
		
		bridging.typename = default("TTBridgingLayer");
		//bridging.typename = default("FGBridgingLayer");
		//bridging.streamIdentifier.typename = default("TSN5GIdentifierLayer");]
		bridging.streamCoder.typename = default("FGCoderLayer");
		//bridging.streamCoder.decoder.typename = default("");
		//bridging.streamCoder.encoder.typename = default("");
		tt[*].delayLayer.delay = default(isDstt ? this.delayD2D : this.delayDownlink);
		tt[0].delayLayer.delay = default(isDstt ? this.delayUplink : this.delayDownlink);
		tt[*].delayLayer.delayQFI = default(this.delayQFI.dup());
        

        tt[*].macLayer.typename = default(hasFramePreemption ? "EthernetPreemptingMacLayer" : "EthernetMacLayer");

        tt[*].macLayer.queue.typename = default(schedule ? "PriorityQueue" : "PacketQueue");
        tt[*].macLayer.queue.numQueues = default(2);
        tt[*].macLayer.queue.classifier.typename = default("T5Classifier");

        tt[*].phyLayer.typename = default(hasCutthroughSwitching ? "EthernetStreamThroughPhyLayer" : (hasFramePreemption ? "EthernetPreemptingPhyLayer" : "EthernetPhyLayer"));
		@display("i=device/card");


    gates:
        inout ttg[numTtInterfaces] @labels(EtherFrame-conn);
    submodules:
        tt[sizeof(ttg)]: <default("TTInterface")> like IEthernetInterface {
            @display("p=1277.64,748.67,row,150;q=txQueue");
        }
        
    connections:
        for i=0..sizeof(ttg)-1 {
            li.out++ --> tt[i].upperLayerIn;
            li.in++ <-- tt[i].upperLayerOut;
            tt[i].phys <--> { @display("m=s"); } <--> ttg[i];
        }
}

/////////////////////////////////////////////////////////////////


