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

package simulations;

import inet.node.ethernet.EthernetLink;
import inet.node.ethernet.Eth10M;
import inet.node.tsn.TsnDevice;

import src.devices.FiveGS;
import src.networkBase.TSN5GD6NetBase;



network SimpleTest extends TSN5GD6NetBase
{
    parameters:
        **.eth*.bitrate = 10Mbps;
    submodules:
        talker: TsnDevice {
            @display("p=579,317");
        }
        FiveG: FiveGS {
            scheduleFiveG = true;
            @display("p=439,317");
        }
        listener: TsnDevice {
            @display("p=336,317");
        }
    connections:
        talker.ethg++ <--> Eth10M <--> FiveG.nwttg++;
        FiveG.dsttg++ <--> Eth10M <--> listener.ethg++;
}

