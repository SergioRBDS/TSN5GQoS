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

#include "T5Classifier.h"

using namespace omnetpp;
using namespace inet;

Define_Module(T5Classifier);

void T5Classifier::initialize(int stage)
{
    PacketClassifierBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {


    }
    else if (stage == INITSTAGE_QUEUEING) {

    }
}

int T5Classifier::classifyPacket(Packet *packet)
{
    for (size_t i = 0; i < consumers.size(); i++) {
        size_t outputGateIndex = getOutputGateIndex(i);
        if (consumers[outputGateIndex]->canPushPacket(packet, outputGates[outputGateIndex]))
            return outputGateIndex;
    }
    return -1;
}
