// This file is part of Deliverable D4.1 DetCom Simulator Framework Release 1
// of the DETERMINISTIC6G project receiving funding from the
// European Union’s Horizon Europe research and innovation programme
// under Grant Agreement No. 101096504.
//
// SPDX-License-Identifier: LGPL-3.0-or-later


#include "FGDelayer.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/networklayer/common/NetworkInterface.h"
#include "src/linklayer/common/QfiTag_m.h"


Define_Module(FGDelayer);

void FGDelayer::initialize(int stage)
{
    PacketDelayerBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        setDelay(&par("delay"));
    }

    if (stage == INITSTAGE_LINK_LAYER) {
        qosMap = check_and_cast<QoSMap*>(getModuleByPath("^.^.^.^.qosMap"));

        setDelays(&par("delayQFI"));
    }
}

void FGDelayer::addInterfacesToSet(std::set<int>& set, const char *interfaceType) {
    // Check if context has submodule with name interfaceType
    auto node = getContainingNode(this);
    if (!node->hasSubmoduleVector(interfaceType)) {
        throw cRuntimeError("No submodule with name '%s' found in '%s'", interfaceType, node->getFullPath().c_str());
    }

    // Get submodule vector with name interfaceType
    for (int i = 0; i < node->getSubmoduleVectorSize(interfaceType); i++) {
        auto *interface = dynamic_cast<NetworkInterface *>(node->getSubmodule(interfaceType, i));
        if (interface == nullptr) {
            throw cRuntimeError("Submodule with name '%s' is not a NetworkInterface", interfaceType);
        }
        set.insert(interface->getInterfaceId());
    }//double delays = delayField.second.doubleValueInUnit("s");

}

clocktime_t FGDelayer::computeDelay(Packet *packet) const
{
    auto context = getContainingNode(this);
    auto tryQfi = packet->findTag<QfiReq>();
    int qfi;
    if(tryQfi != nullptr){
        qfi = tryQfi->getQfi();
        auto delayMap = check_and_cast<cValueMap *>(delayParameters->objectValue());
        double currentDelay = delayMap->get(to_string(qfi).c_str()).doubleValueInUnit("s");
        EV << "QFI: " << qfi << ", Delay: " << currentDelay << endl;
        return currentDelay;
        //auto delayFields = delayMap->getFields();
        //double delayGoal = 0;
        //for (auto &delayField : delayFields) {
        //    EV << "QFI: " << stoi(delayField.first) << ", Delay: " << delayField.second.doubleValueInUnit("s") << endl;
        //    if(stoi(delayField.first)==qfi)
        //        delayGoal = delayField.second.doubleValueInUnit("s");
        //}EV << delayGoal << endl;
        //return delayGoal;
    }
    auto delayMap = check_and_cast<cValueMap *>(delayParameters->objectValue());
    double currentDelay = delayMap->get(to_string(87).c_str()).doubleValueInUnit("s");
    EV << "QFI: " << 87 << ", Delay: " << endl;
    return currentDelay;
    return delayParameter->doubleValue();
}

void FGDelayer::setDelay(cPar *delay) {
    EV << "setDelay " << endl;
    delayParameter = delay;
}

void FGDelayer::setDelays(cPar *delays) {
    EV << "setDelay " << endl;
    delayParameters = delays;

/*
    auto delayMap = check_and_cast<cValueMap *>(delays->objectValue());
    auto delayFields = delayMap->getFields();
    for (auto &delayField : delayFields) {
        int qfi = stoi(delayField.first);
        //double delays = delayField.second.doubleValueInUnit("s");
            //EV << "OBJETO " << stoi(delayField.first) << " " << double(delayField.second.doubleValueInUnit("s")) << endl;
            //EV << "OBJETO " << stoi(delayField.first) << " " << delayField.second << endl;
        //cPar& aux = check_and_cast<cPar& >(delayField.second);
        //delayParameters[stoi(delayField.first)] = double(delayField.second.doubleValueInUnit("s"));
        delayParameters[stoi(delayField.first)] = delayField.second;
        EV << "QFI: " << qfi << ", Delay: " << delayField.second.doubleValueInUnit("s") << endl;
    }
*/

}

void FGDelayer::handleParameterChange(const char *parname) {
    if (!strcmp(parname, "delay")) {
        EV << "delay " << endl;
        setDelay(&par("delay"));
        setDelays(&par("delayQFI"));
    }
    if (!strcmp(parname, "delayQFI")) {
        EV << "delayQFI " << endl;
        setDelay(&par("delay"));
        setDelays(&par("delayQFI"));
    }
}
