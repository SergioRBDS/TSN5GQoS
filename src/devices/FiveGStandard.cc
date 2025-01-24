// This file is part of Deliverable D4.1 DetCom Simulator Framework Release 1
// of the DETERMINISTIC6G project receiving funding from the
// European Union’s Horizon Europe research and innovation programme
// under Grant Agreement No. 101096504.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
// 

#include "FiveGStandard.h"
#include "inet/networklayer/configurator/base/L3NetworkConfiguratorBase.h"

using namespace inet;

Define_Module(FiveGStandard);

void FiveGStandard::initialize(int stage) {
    cComponent::initialize(stage);
    if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {
        EV << "Inicializando FiveGStandard" << endl;
        cXMLElement *configEntity = par("delayConfig");
        // parse XML config
        if (strcmp(configEntity->getTagName(), "delays") != 0) {
            throw cRuntimeError("Cannot read delay configuration, unaccepted '%s' entity at %s",
                                configEntity->getTagName(),
                                configEntity->getSourceLocation());
        }

        cXMLElementList uplinkEntities = configEntity->getChildrenByTagName("uplink");
        for (auto &uplinkEntity: uplinkEntities) {
            this->par("delayUplink").parse(uplinkEntity->getNodeValue());
            EV << "Dados de UPLINK carregados" << endl;
        }

        cXMLElementList downlinkEntities = configEntity->getChildrenByTagName("downlink");
        for (auto &downlinkEntity: downlinkEntities) {
            this->par("delayDownlink").parse(downlinkEntity->getNodeValue());
            EV << "Dados de UPLINK carregados" << endl;
        }
    }
}
