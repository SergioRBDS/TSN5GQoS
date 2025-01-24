// This file is part of Deliverable D4.1 DetCom Simulator Framework Release 1
// of the DETERMINISTIC6G project receiving funding from the
// European Union’s Horizon Europe research and innovation programme
// under Grant Agreement No. 101096504.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
// 

#include "FiveGS.h"
#include "inet/networklayer/configurator/base/L3NetworkConfiguratorBase.h"

using namespace inet;
using namespace std;

Define_Module(FiveGS);

void FiveGS::initialize(int stage) {
    /*
    cComponent::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        verifyTT("dstt");
        verifyTT("nwtt");
    }
    */
}

cModule *FiveGS::getDstt(cXMLElement *delayEntity) const {
    const char *deviceAttr = delayEntity->getAttribute("device");
    if (deviceAttr == nullptr) {
        throw cRuntimeError("Invalid delay configuration, missing 'device' attribute at %s",
                            delayEntity->getSourceLocation());
    }
    L3NetworkConfiguratorBase::Matcher deviceMatcher(deviceAttr);
    for (int i = 0; i < getSubmoduleVectorSize("dstt"); i++) {
        auto dstt = getSubmodule("dstt", i);
        if (dstt->getSubmoduleVectorSize("eth") == 0) {
            throw cRuntimeError("Invalid delay configuration, no 'eth' interface found at %s",
                                dstt->getFullPath().c_str());
        }
        auto eth = dynamic_cast<NetworkInterface *>(dstt->getSubmodule("eth", 0));
        std::string deviceName = eth->getRxTransmissionChannel()->getSourceGate()->getOwnerModule()->getFullName();
        if (deviceMatcher.matches(deviceName.c_str())) {
            return dstt;
        }
    }
    throw cRuntimeError("Invalid delay configuration, no device found for '%s' at %s",
                        deviceAttr, delayEntity->getSourceLocation());
}

void FiveGS::verifyTT(string TT){
    int numTTs = getSubmodule(TT.c_str(), 0)->getVectorSize();
    for(int i=0;i<numTTs;i++){
        cModule *TTmodule = getSubmodule(TT.c_str(), i);
        if(TTmodule){
            EV << TT << " encontrado" << endl;
            cModule *bridgingLayer = TTmodule->getSubmodule("bridging");
            if (bridgingLayer) {
                cModule *streamIdentifier = bridgingLayer->getSubmodule("streamIdentifier");
                if (streamIdentifier) {
                    EV << "streamIdentifier encontrado em " << TTmodule->getFullPath() << ": " << streamIdentifier->getFullPath() << endl;
                }
            }
        }
    }
}


/*
cModule *FiveGS::getNwtt(cXMLElement *delayEntity) const {
    const char *deviceAttr = delayEntity->getAttribute("device");
    if (deviceAttr == nullptr) {
        throw cRuntimeError("Invalid delay configuration, missing 'device' attribute at %s",
                            delayEntity->getSourceLocation());
    }
    L3NetworkConfiguratorBase::Matcher deviceMatcher(deviceAttr);
    auto nwtt = getSubmodule("nwtt");
    if (nwtt->getSubmoduleVectorSize("eth") == 0) {
        throw cRuntimeError("Invalid delay configuration, no 'eth' interface found at %s",
                            nwtt->getFullPath().c_str());
    }
    auto eth = dynamic_cast<NetworkInterface *>(nwtt->getSubmodule("eth", 0));
    std::string deviceName = eth->getRxTransmissionChannel()->getSourceGate()->getOwnerModule()->getFullName();
    if (deviceMatcher.matches(deviceName.c_str())) {
        return nwtt;
    }

    throw cRuntimeError("Invalid delay configuration, no device found for '%s' at %s",
                        deviceAttr, delayEntity->getSourceLocation());
}
*/
