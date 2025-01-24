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

#ifndef SRC_QUEUEING_CLASSIFIER_T5CLASSIFIER_H_
#define SRC_QUEUEING_CLASSIFIER_T5CLASSIFIER_H_

#include "inet/queueing/base/PacketClassifierBase.h"

#include<map>
#include<string>
#include<vector>


using namespace omnetpp;
using namespace inet;

class T5Classifier : public queueing::PacketClassifierBase{
protected:
    virtual void initialize(int stage) override;
    virtual int classifyPacket(Packet *packet) override;
public:


};

#endif /* SRC_QUEUEING_CLASSIFIER_T5CLASSIFIER_H_ */
