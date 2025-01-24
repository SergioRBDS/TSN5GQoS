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

package src.gPTP;

import inet.applications.contract.IApp;

simple GptpTT like IApp
{
    parameters:
        int domainNumber = default(0); // specifies the time domain number used in gPTP messages
        double syncInterval @unit(s) = default(0.125s); // time interval between SYNC messages
        double pDelayReqProcessingTime @unit(s) = default(8us);  // processing time between arrived PDelayReq and send of PDelayResp
        double pdelayInitialOffset @unit(s) = default(0s); // time of first link delay measurement
        double pdelayInterval @unit(s) = default(1s); // frequency of link delay measurements
        
        string slavePort = default(""); // port for receiving time (empty for MASTER_NODE)
        object masterPorts = default([]); // ports for sending out time (empty for SLAVE_NODE)

		string interfaceTableModule;
		string clockModule = default(""); 
		
		@display("i=block/timer");
		@signal[rateRatio](type=double);
		@signal[sequenceID](type=int);
		@signal[peerDelay](type=simtime_t);
        @statistic[peerDelay](record=vector; interpolationmode=sample-hold);
        @statistic[sequenceID](record=vector; interpolationmode=sample-hold);
        @statistic[rateRatio](record=vector; interpolationmode=sample-hold);
        
		@selfMessageKinds(inet::GptpSelfMsgKind);
    gates:
        input socketIn;
        output socketOut;
}

