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

package simulations.TesteOnly;

import inet.node.ethernet.EthernetLink;
import inet.node.ethernet.Eth10M;
import src.networkBase.TSN5GD6NetBase;
import src.devices.FiveGS;
import src.devices.FiveGSv2;

import inet.node.tsn.TsnDevice;
import inet.node.tsn.TsnSwitch;

network TestQoS2 extends TSN5GD6NetBase
{
    parameters:
        **.eth*.bitrate = 10Mbps;
        @display("bgb=1731.0719,890.62396");
    submodules:
        switch5GC: TsnSwitch;
        PLC1: TsnDevice;
        PLC2: TsnDevice;
        PLC3: TsnDevice;
        SafetyController: TsnDevice;
        AGVControlCenter: TsnDevice;
        PredictiveMaintenance: TsnDevice;
        switchCell1: TsnSwitch;
        SensorPosition1: TsnDevice;
        Actuator1: TsnDevice;
        VibrationSensor: TsnDevice;
        Camera1: TsnDevice;
        QualitySystem: TsnDevice;
        switchCell2: TsnSwitch;
        RobotSafetyModule: TsnDevice;
        switchCell3: TsnSwitch;
        AGVRobot: TsnDevice;
        GasSensor1: TsnDevice;
        FanController2: TsnDevice;
        switchCell4: TsnSwitch;
        TemperatureSensor1: TsnDevice;
        HumiditySensor1: TsnDevice;
        FanController: TsnDevice;
        VibrationSensor2: TsnDevice;
        FG: FiveGSv2 {
            @display("p=974.84796,455.168");
        }
    connections:
        switchCell1.ethg++ <--> Eth10M <--> FG.dsttg++;
        switchCell1.ethg++ <--> Eth10M <--> SensorPosition1.ethg++;
        switchCell1.ethg++ <--> Eth10M <--> Actuator1.ethg++;
        switchCell1.ethg++ <--> Eth10M <--> VibrationSensor.ethg++;
        switchCell1.ethg++ <--> Eth10M <--> Camera1.ethg++;
        switchCell1.ethg++ <--> Eth10M <--> QualitySystem.ethg++;


        switchCell2.ethg++ <--> Eth10M <--> FG.dsttg++;
        switchCell2.ethg++ <--> Eth10M <--> RobotSafetyModule.ethg++;

        switchCell3.ethg++ <--> Eth10M <--> FG.dsttg++;
        switchCell3.ethg++ <--> Eth10M <--> AGVRobot.ethg++;
        switchCell3.ethg++ <--> Eth10M <--> GasSensor1.ethg++;
        switchCell3.ethg++ <--> Eth10M <--> FanController2.ethg++;

        switchCell4.ethg++ <--> Eth10M <--> FG.dsttg++;
        switchCell4.ethg++ <--> Eth10M <--> TemperatureSensor1.ethg++;
        switchCell4.ethg++ <--> Eth10M <--> HumiditySensor1.ethg++;
        switchCell4.ethg++ <--> Eth10M <--> FanController.ethg++;
        switchCell4.ethg++ <--> Eth10M <--> VibrationSensor2.ethg++;


        FG.nwttg++ <--> Eth10M <--> switch5GC.ethg++;
        PLC1.ethg++ <--> Eth10M <--> switch5GC.ethg++;
        PLC2.ethg++ <--> Eth10M <--> switch5GC.ethg++;
        PLC3.ethg++ <--> Eth10M <--> switch5GC.ethg++;
        SafetyController.ethg++ <--> Eth10M <--> switch5GC.ethg++;
        AGVControlCenter.ethg++ <--> Eth10M <--> switch5GC.ethg++;
        PredictiveMaintenance.ethg++ <--> Eth10M <--> switch5GC.ethg++;
}
