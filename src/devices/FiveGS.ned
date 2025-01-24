package src.devices;

import src.devices.tsntranslator.TTChannel;
import src.devices.tsntranslator.TT;
import src.devices.FiveGStandard;

import src.devices.QoSMap;

module FiveGS
{
    parameters:
        bool scheduleFiveG = default(false);
        int numUEs = default(1); // Number of connected UEs (specifies number of dstt modules).
        int numUPFs = default(1); // Number of connected UEs (specifies number of dstt modules).
        xml delayConfig = default(xml("<delays></delays>")); // XML delay configuration, alternative way to specify the uplink and downlink delays for dstts.

        @display("i=device/antennatower;bgb=bgbW,265");
        @class(FiveGS);
        int bgbW = 350;
    gates:
        inout dsttg[numUEs];
        inout nwttg[numUPFs];
    submodules:
        qosMap: QoSMap {
            @display("p=552,41");
        }
        dstt[sizeof(dsttg)]: TT {
            //schedule = parent.scheduleFiveG;
            isDstt = default(true);
            @display("p=75,211,row,50");
        }
        fiveGSystem: FiveGStandard {
            numDSTT = 2;
            @display("p=297,125;is=vl");
        }
        nwtt[sizeof(nwttg)]: TT {
            schedule = parent.scheduleFiveG;
            @display("p=75,41,row,50");
        }
    connections:
        for i=0..sizeof(dsttg)-1 {
            dsttg[i] <--> dstt[i].ethg++;
            dstt[i].ttg++ <--> TTChannel <--> fiveGSystem.ttg++;
        }
        for i=0..sizeof(nwttg)-1 {
            nwttg[i] <--> nwtt[i].ethg++;
            nwtt[i].ttg++ <--> TTChannel <--> fiveGSystem.ttg++;
        }


}


module FiveGSv2
{
    parameters:
        bool scheduleFiveG = default(false);
        int numUEs = default(1); // Number of connected UEs (specifies number of dstt modules).
        int numUPFs = default(1); // Number of connected UEs (specifies number of dstt modules).
        xml delayConfig = default(xml("<delays></delays>")); // XML delay configuration, alternative way to specify the uplink and downlink delays for dstts.

        @display("i=device/antennatower;bgb=bgbW,265");
        @class(FiveGS);
        int bgbW = 350;
    gates:
        inout dsttg[numUEs];
        inout nwttg[numUPFs];
    submodules:
        qosMap: QoSMap {
            @display("p=283,41");
        }
        dstt[sizeof(dsttg)]: TT {
            //schedule = parent.scheduleFiveG;
            isDstt = default(true);
            @display("p=75,211,row,50");
        }
        nwtt[sizeof(nwttg)]: TT {
            schedule = parent.scheduleFiveG;
            @display("p=75,41,row,50");
        }
    connections:
        for i=0..sizeof(dsttg)-1 {
            dsttg[i] <--> dstt[i].ethg++;
        }
        for i=0..sizeof(nwttg)-1 {
            nwttg[i] <--> nwtt[i].ethg++;
        }

        for i=0..sizeof(dsttg)-1, for j=0..sizeof(nwttg)-1 {
            dstt[i].ttg++ <--> TTChannel <--> nwtt[j].ttg++;
        }
        for i=0..sizeof(dsttg)-1, for j=i+1..sizeof(dsttg)-1 {
            dstt[i].ttg++ <--> TTChannel <--> dstt[j].ttg++;
        }

}


