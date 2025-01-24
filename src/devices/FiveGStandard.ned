//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package src.devices;

import inet.node.tsn.TsnSwitch;
import inet.linklayer.contract.IEthernetInterface;

module FiveGStandard extends TsnSwitch
{
    parameters:
        int numDSTT = default(1);
        volatile double delayUplink @mutable @unit(s) = default(0s); // Uplink Delay
        volatile double delayDownlink @mutable @unit(s) = default(0s); // Downlink Delay
        xml delayConfig = default(xml("<delays></delays>"));
        bridging.typename = default("FGBridgingLayer");

        //bridging.delayLayer.typename = default("FGDelayLayer");
        bridging.delayLayer.delayUplink = default(this.delayUplink);
        bridging.delayLayer.delayDownlink = default(this.delayDownlink);
        @class(FiveGStandard);

    gates:
        inout ttg[numDSTT+1] @labels(EtherFrame-conn);
    submodules:
        dstt[numDSTT]: <default("TTInterface")> like IEthernetInterface {
            @display("p=1277.64,748.67,row,150;q=txQueue");
        }
        nstt: <default("TTInterface")> like IEthernetInterface {
            @display("p=250,748,row,150;q=txQueue");
        }
    connections:
        li.out++ --> nstt.upperLayerIn;
        li.in++ <-- nstt.upperLayerOut;
        nstt.phys <--> { @display("m=s"); } <--> ttg[0];
        for i=0..numDSTT-1 {
            li.out++ --> dstt[i].upperLayerIn;
            li.in++ <-- dstt[i].upperLayerOut;
            dstt[i].phys <--> { @display("m=s"); } <--> ttg[i+1];
        }
}
