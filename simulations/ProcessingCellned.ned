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

import inet.networks.base.TsnNetworkBase;
import inet.node.contract.IEthernetNetworkNode;
import inet.node.ethernet.EthernetLink;

network ProcessingCell
{
	gates:
	    inout FiveGConnection;
	submodules:
	    bridge1: <default("TsnSwitch")> like IEthernetNetworkNode;
	    bridge2: <default("TsnSwitch")> like IEthernetNetworkNode;
	    swarmStatus: <default("TsnDevice")> like IEthernetNetworkNode;
	    swarmControl: <default("TsnDevice")> like IEthernetNetworkNode;
	connections:
	    swarmControl.ethg++ <--> EthernetLink <--> bridge1.ethg++;
	    swarmStatus.ethg++ <--> EthernetLink <--> bridge2.ethg++;
	    bridge1.ethg++ <--> EthernetLink <--> bridge2.ethg++;
	    bridge1.ethg++ <--> FiveGConnection;
}


