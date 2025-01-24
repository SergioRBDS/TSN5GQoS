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

package simulations.TimeSynchronization;


import inet.node.ethernet.EthernetLink;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100M;
import inet.node.tsn.TsnDevice;
import inet.node.tsn.TsnSwitch;

import src.devices.FiveGS;
import src.devices.FiveGSv2;
import src.networkBase.TSN5GD6NetBase;
import src.devices.tsntranslator.TT;


network TimeSyncDLUL extends TSN5GD6NetBase
{
    parameters:
        **.eth*.bitrate = 100Mbps;
        @display("bgb=1231.932,888.63605");
    submodules:
        TsnSwitchDomain1: TsnDevice {
            @display("p=700,675");
        }
        //TsnSwitchDomain2: TsnDevice {
        //    @display("p=852.87604,550.70404");
        //}
        TsnSwitchDataNetwork: TsnDevice {
            @display("p=700,225");
        }
        FGS: FiveGSv2 {
            @display("p=700,450");
        }
    connections:
        TsnSwitchDomain1.ethg++ <--> Eth100M <--> FGS.dsttg++;
        //TsnSwitchDomain2.ethg++ <--> Eth10M <--> FGS.dsttg++;
        TsnSwitchDataNetwork.ethg++ <--> Eth100M <--> FGS.nwttg++;
        //TsnSwitchDataNetwork.ethg++ <--> Eth10M <--> TsnSwitchDomain1.ethg++;
        //TsnSwitchDomain1.ethg++ <--> Eth10M <--> FGS.ethg++;
        //TsnSwitchDataNetwork.ethg++ <--> Eth10M <--> FGS.ethg++;
}


network TimeSyncUEUE extends TSN5GD6NetBase
{
    parameters:
        **.eth*.bitrate = 10Mbps;
        @display("bgb=1231.932,888.63605");
    submodules:
        TsnSwitchDomain1: TsnDevice {
            @display("p=450,675");
        }
        TsnSwitchDomain2: TsnDevice {
            @display("p=950,675");
        }
        TsnSwitchDataNetwork: TsnDevice { //TsnSwitch {
            @display("p=700,225");
        }
        //TsnDeviceDataNetwork: TsnDevice {
        //    @display("p=925,125");
        //}
        FGS: FiveGSv2 {
            @display("p=700,450");
        }
    connections:
        TsnSwitchDomain1.ethg++ <--> Eth10M <--> FGS.dsttg++;
        TsnSwitchDomain2.ethg++ <--> Eth10M <--> FGS.dsttg++;
        TsnSwitchDataNetwork.ethg++ <--> Eth10M <--> FGS.nwttg++;
        //TsnSwitchDataNetwork.ethg++ <--> Eth10M <--> TsnDeviceDataNetwork.ethg++;
        
}

