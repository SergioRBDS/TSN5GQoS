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

package simulations.TSNonly2;

import inet.node.ethernet.EthernetLink;
import inet.node.ethernet.Eth10M;
import src.networkBase.TSN5GD6NetBase;
import src.devices.FiveGS;
import src.devices.FiveGSv2;

import inet.node.tsn.TsnDevice;
import inet.node.tsn.TsnSwitch;


network TestGptpQoS2 extends TSN5GD6NetBase
{
    parameters:
        **.eth*.bitrate = 10Mbps;
        @display("bgb=1731.0719,890.62396");
    submodules:
        switch5GC: TsnDevice {

            @display("p=876.175,195.1875");
        }
        switchCell1: TsnDevice {


            @display("p=730.8688,496.64377");
        }
        switchCell2: TsnDevice {

            @display("p=1058.35,470.61874");
        }
        FGS: FiveGSv2 {
            @display("p=876.175,366.51877");
        }
    connections:
        switchCell1.ethg++ <--> Eth10M <--> FGS.dsttg++;

        switchCell2.ethg++ <--> Eth10M <--> FGS.dsttg++;


        FGS.nwttg++ <--> Eth10M <--> switch5GC.ethg++;
}
