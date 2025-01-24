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

package simulations.UseCase;

import inet.node.ethernet.EthernetLink;
import inet.node.ethernet.Eth10M;
import src.networkBase.TSN5GD6NetBase;
import src.devices.FiveGS;
import src.devices.FiveGSv2;

import inet.node.tsn.TsnDevice;
import inet.node.tsn.TsnSwitch;


network TestGptpQoS extends TSN5GD6NetBase
{
    parameters:
        **.eth*.bitrate = 10Mbps;
        @display("bgb=1731.0719,890.62396");
    submodules:
        
        SwitchS1: TsnSwitch;
        SensorS1: TsnDevice;
        CameraS1: TsnDevice;
        EsteiraS1: TsnDevice;
        RoboS1: TsnDevice;
        
        SwitchS2: TsnSwitch;
        EsteiraS2: TsnDevice;
        RoboS2: TsnDevice;
        
        SwitchS3: TsnSwitch;
        SensorS3: TsnDevice;
        EsteiraS3: TsnDevice;
        
        SwitchDN: TsnSwitch {
            @display("p=1106.0625,451.1");
        }
        PLC: TsnDevice {

            @display("p=1231.85,453.26877");
        }
        StorageDevice: TsnDevice;
        
        FGS: FiveGSv2 {
            @display("p=969.4313,453.26877");
        }
    connections:

        SwitchS1.ethg++ <--> Eth10M <--> FGS.dsttg++;
        SwitchS1.ethg++ <--> Eth10M <--> SensorS1.ethg++;
        SwitchS1.ethg++ <--> Eth10M <--> CameraS1.ethg++;
        SwitchS1.ethg++ <--> Eth10M <--> EsteiraS1.ethg++;
        SwitchS1.ethg++ <--> Eth10M <--> RoboS1.ethg++;
        
        SwitchS2.ethg++ <--> Eth10M <--> FGS.dsttg++;        
        SwitchS2.ethg++ <--> Eth10M <--> EsteiraS2.ethg++;
        SwitchS2.ethg++ <--> Eth10M <--> RoboS2.ethg++;
        
        SwitchS3.ethg++ <--> Eth10M <--> FGS.dsttg++;
        SwitchS3.ethg++ <--> Eth10M <--> SensorS3.ethg++;
        SwitchS3.ethg++ <--> Eth10M <--> EsteiraS3.ethg++;

        SwitchDN.ethg++ <--> Eth10M <--> FGS.nwttg++;
        SwitchDN.ethg++ <--> Eth10M <--> PLC.ethg++;
        StorageDevice.ethg++ <--> Eth10M <--> PLC.ethg++;
}
