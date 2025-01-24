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

package simulations.QoS;

import inet.node.ethernet.Eth10M;
import src.networkBase.TSN5GD6NetBase;
import src.devices.FiveGSv2;

import inet.node.tsn.TsnDevice;

network TestQoS extends TSN5GD6NetBase
{
    parameters:
        **.eth*.bitrate = 10Mbps;
        @display("bgb=699,892");
    submodules:
        T: TsnDevice {
            @display("p=374,206,col,100");
        }
        L: TsnDevice {
            @display("p=589,270");
        }
        FG: FiveGSv2 {
            @display("p=486,265");
        }
    connections:
        T.ethg++ <--> Eth10M <--> FG.dsttg++;
    
        FG.nwttg++ <--> Eth10M <--> L.ethg++;
}
