/*
 * QoSMap.cpp
 *
 *  Created on: Jul 14, 2024
 *      Author: pregui
 */

#include "QoSMap.h"

Define_Module(QoSMap);

#include <algorithm>

using namespace std;
using namespace omnetpp;
using namespace inet;


void QoSMap::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        EV << "QoSMAP::INITSTAGE_LOCAL" << endl;
        cXMLElement* root5QI = par("FQITableFile");
        cXMLElement* rootApps = par("TSNAppsTableFile");
        Apps = new cValueMap();
        BE = readPCPs("BE");
        AVB = readPCPs("AVB");
        ST = readPCPs("ST");
        par("Apps").setObjectValue(Apps);
        ctrlParams = check_and_cast<cValueArray*>(par("controlParameters").objectValue());
        read5QIFile(root5QI);
        readTSNAppsFile(rootApps);
        mappingQoS();
        printMap();
    }
}

vector<int>* QoSMap::readPCPs(string AppType){
    cValueArray* valueArray = check_and_cast<cValueArray*>(par(AppType.c_str()).objectValue());
    vector<int>* vec = new vector<int>();
    for (int i = 0; i < valueArray->size(); ++i) {
        int pcp = valueArray->get(i).intValue();
        Apps->set(to_string(pcp).c_str(), new cArray(to_string(pcp).c_str())); // Adiciona ao mapa
        vec->push_back(pcp);
    }sort(vec->begin(), vec->end(), greater<int>());
    return vec;
}



void QoSMap::read5QIFile(cXMLElement* root)
{
    for (const auto& type : FGFlowType){
        if (cXMLElement* elem = root->getFirstChildWithTag(type.c_str())){
            cXMLElementList flowList = elem->getChildrenByTagName("TrafficType");
            vector<FiveQI>* qfiList = new vector<FiveQI>();
            for(auto& flow: flowList){
                FiveQI trafficData;
                const cXMLAttributeMap params = flow->getAttributes();
                for (const auto& [name, value] : params){
                    trafficData[name] = stoi(value);
                }
                qfiList->push_back(trafficData);
            }
            QFI_table[type] = qfiList;
        }
    }
}

void QoSMap::readTSNAppsFile(cXMLElement* root)
{
    cXMLElementList apps = root->getChildrenByTagName("app");
    for(auto& app: apps){
        App* nextApp = new App();
        cXMLElement* pktLengthElem = app->getFirstChildWithAttribute("param", "name", "packetLength");
        cXMLElement* intervallElem = app->getFirstChildWithAttribute("param", "name", "productionInterval");
        cXMLElement* pcpElem = app->getFirstChildWithAttribute("param", "name", "pcp");
        if (pktLengthElem && intervallElem && pcpElem) {
            if (Apps->get(pcpElem->getNodeValue()).getType() == cValue::POINTER){
                cValueMap* newApp = new cValueMap();
                newApp->set("pktLength",cValue::parseQuantity(pktLengthElem->getNodeValue(),"B"));
                newApp->set("intervall",cValue::parseQuantity(intervallElem->getNodeValue(),"s"));
                for (int i = 0; i < ctrlParams->size(); ++i) {
                    cXMLElement* p = app->getFirstChildWithAttribute("param", "name", ctrlParams->get(i).stringValue());
                    if(p){
                        newApp->set(ctrlParams->get(i).stringValue(),stod(p->getNodeValue()));
                    }
                }
                cArray* oldApps = dynamic_cast<cArray*>(Apps->get(pcpElem->getNodeValue()).objectValue());
                oldApps->add(newApp);
            }
        }
    }
}

void QoSMap::mappingQoS(){

    EV << "QoSMap:: Mapeamento" << endl;
    mappingDCGBR(QFI_table["DCGBR"]);
    mappingGBR(QFI_table["GBR"]);
    mappingNonGBR(QFI_table["NonGBR"]);

}

void QoSMap::mappingGBR(vector<FiveQI>* GBR){
    // ORDENAR
    sort(GBR->begin(), GBR->end(),[](const auto& a, const auto& b) {
        if (a.at("PDB") != b.at("PDB"))
            return a.at("PDB") < b.at("PDB");
        return a.at("PER") > b.at("PER");
    });
    EV << "GBR: " << endl;
    // Separar por PCP
    map<int,vector<pair<int,double>>> apps2Map; // veor com as aplicações a serem mapeadas (separadas por par pcp e posição da aplicação no pcp)
    int totalApps = 0;
    for (int pcp : *AVB){
        vector<pair<int,double>> Pcp2FQI;
        cArray* pcpApps = dynamic_cast<cArray*>(Apps->get(to_string(pcp).c_str()).objectValue());
        EV << "Valores(PCP=" << pcp <<"): ";
        for (int i = 0; i < pcpApps->size(); i++){
            cValueMap *app = check_and_cast<cValueMap *>(pcpApps->get(i));
            if(app->containsKey("latency")){
                Pcp2FQI.push_back(make_pair(i,app->get("latency")));
                EV << app->get("latency").doubleValue()/1000 << " - ";
            }
            else if(app->containsKey("bandwidth")){
                Pcp2FQI.push_back(make_pair(i,8*app->get("pktLength").doubleValue()/(app->get("bandwidth").doubleValue())/1000000));
                EV << 8*app->get("pktLength").doubleValue()/(app->get("bandwidth").doubleValue())/1000000 << " - ";
            }
            totalApps++;
        }EV << endl;
        if(!Pcp2FQI.empty()){
            apps2Map[pcp] = Pcp2FQI;
        }
    }

    if(apps2Map.empty()){
        return;
    }

    int TotalGbrFlows = GBR->size(); //12
    map<int,int> PcpAppsPer5QI;
    vector<pair<int,double>> remainder;
    int appsSum = 0;
    for(const auto& [pcp, _] : apps2Map){
        double appsPer5QI = double(apps2Map[pcp].size()) * TotalGbrFlows / totalApps;
        int intVal = static_cast<int>(round(appsPer5QI));
        PcpAppsPer5QI[pcp] = intVal;
        remainder.push_back(make_pair(pcp,appsPer5QI));
        appsSum += intVal;
        EV << intVal << "-classes |numApps: " << apps2Map[pcp].size()<< endl;
    }

    int diferenca = TotalGbrFlows - appsSum;
    if (diferenca != 0) {
        sort(remainder.begin(), remainder.end(), [](const auto& a, const auto& b) {
            return fabs(a.second) > fabs(b.second);
        });

        for (int i = 0; i < abs(diferenca); ++i) {
            PcpAppsPer5QI[remainder[i].first] += (diferenca > 0) ? 1 : -1;
        }
    }


    int aux = 0;
    for(const auto& [pcp,appList] : apps2Map){
        allocateAppsToQfi2(appList,pcp,aux,PcpAppsPer5QI[pcp]);
        aux += PcpAppsPer5QI[pcp];
        printMap();
    }
}



void QoSMap::mappingNonGBR(vector<FiveQI>* NGBR){
    // ORDENAR
    sort(NGBR->begin(), NGBR->end(),[](const auto& a, const auto& b) {
        if (a.at("PER") != b.at("PER"))
            return a.at("PER") > b.at("PER");
        return a.at("PDB") < b.at("PDB");
    });

    // Separar por PCP
    map<int,vector<pair<int,double>>> apps2Map;
    int totalApps = 0;
    for (int pcp : *BE){
        cArray* appsNgbr = dynamic_cast<cArray*>(Apps->get(to_string(pcp).c_str()).objectValue());
        vector<pair<int,double>> noPER;
        vector<pair<int,double>> lowPER;
        vector<pair<int,double>> PER;
        EV << "QoSMap:: Mapeamento Non GBR" << endl;
        for (int i = 0; i < appsNgbr->size(); i++) {
            cValueMap *app = check_and_cast<cValueMap *>(appsNgbr->get(i));
            if(app->containsKey("PER")){
                if(app->get("PER").doubleValue()<(1e-3)){
                    lowPER.push_back(make_pair(i,app->get("intervall")));
                }else if(app->get("PER").doubleValue()<(1e-2)){
                    PER.push_back(make_pair(i,app->get("intervall")));
                }
                else{
                    noPER.push_back(make_pair(i,app->get("intervall")));
                }
            }
            else{
                noPER.push_back(make_pair(i,app->get("intervall")));
            }
            totalApps++;
        }
        allocateAppsToQfi(lowPER,"NonGBR",6,pcp);
        printMap();
        allocateAppsToQfi(PER,"NonGBR",3,pcp);
        printMap();
        allocateAppsToQfi(noPER,"NonGBR",0,pcp);
        printMap();
    }
}



void QoSMap::mappingDCGBR(vector<FiveQI>* DCGBR){
    sort(DCGBR->begin(), DCGBR->end(),[](const auto& a, const auto& b) {
        if (a.at("PDB") != b.at("PDB"))
            return a.at("PDB") < b.at("PDB");
        return a.at("MDBV") > b.at("MDBV");
    });
    EV << "DC-GBR: " << endl;
    map<int,vector<pair<int,double>>> apps2Map; // map <pcp,vector<pos,reqVal>
    int totalApps = 0; // só conta aplicações mapiaveis
    for (int pcp : *ST){ // ST são os valores pcp de aplicações ST
        cArray* pcpApps = dynamic_cast<cArray*>(Apps->get(to_string(pcp).c_str()).objectValue()); // pega as aplicações com pcp igual
        vector<pair<int,double>> Pcp2FQI; // vector <pos,reqVal>
        EV << "Valores: ";
        for (int i = 0; i < pcpApps->size(); i++){
            cValueMap *app = check_and_cast<cValueMap *>(pcpApps->get(i)); // pega a aplicação
            if(app->containsKey("deadline")){
                Pcp2FQI.push_back(make_pair(i,app->get("deadline").doubleValue()/1000));
                EV << app->get("deadline").doubleValue()/1000 << "|";
                totalApps++;
            }
        }EV << endl;
        if(!Pcp2FQI.empty()){
            apps2Map[pcp] = Pcp2FQI;
        }
    }
    if(!apps2Map.empty()){
        int TotalDCFlows = QFI_table["DCGBR"]->size(); //9
        map<int,int> AppsPerPcp;
        vector<pair<int,double>> decQfiPCP;
        int somaDist = 0;
        for(const auto& [pcp, appsList] : apps2Map){
            double aux = double(appsList.size()) * TotalDCFlows / totalApps;
            int intVal = static_cast<int>(round(aux));
            pair<int,double> decVal = make_pair(pcp,aux);
            AppsPerPcp[pcp] = intVal;
            decQfiPCP.push_back(decVal);
            EV << intVal << "-5QI |numApps: " << apps2Map[pcp].size() << "||PCP:" << pcp<< endl;
            somaDist += intVal;
        }
        int diferenca = TotalDCFlows - somaDist;
        if (diferenca != 0) {
            EV << "Deu diferença: " << diferenca << endl;
            std::sort(decQfiPCP.begin(), decQfiPCP.end(), [](const auto& a, const auto& b) {
                return std::fabs(a.second) > std::fabs(b.second);
            });
            for (int i = 0; i < abs(diferenca); i++) {
                int indice = decQfiPCP[i].first;
                EV << "indice = decQfiPCP[i].first; " << indice << endl;
                AppsPerPcp[indice] += (diferenca > 0) ? 1 : -1;
                EV << "AppsPerPcp[indice] += (diferenca > 0) ? 1 : -1; " << AppsPerPcp[indice] << endl;
            }
            EV << "ATT" << endl;
            for(const auto& [pcp, _] : apps2Map){
                EV << AppsPerPcp[pcp] << "-classes |numApps: " << apps2Map[pcp].size() << "||PCP:" << pcp<< endl;
            }
        }
        int aux = 0;
        for(const auto& [pcp,appList] : apps2Map){
            allocateAppsToQfi3(appList,pcp,aux,AppsPerPcp[pcp]);
            aux += AppsPerPcp[pcp];
            EV << aux << " - VAlor aux" << endl;
            printMap();
        }
    }

}


int QoSMap::getPcpFromQfi(int i){

    for (const auto& entry : mapQoS) {
        if (entry.first == i) {
            return entry.second.first;
        }
    }return -1;

}
int QoSMap::getQfiFromPcp(int i){
    for (const auto& entry : mapQoS) {
        if (entry.second.first == i) {
            return entry.first;
        }
    }return -1;
}


void QoSMap::printMap(){
    EV << "-----------------------------" << endl;
    for (const auto& pair : mapQoS) {
        EV << "QFI: " << pair.first << ", PCP: " << pair.second.first << ", APPs: ";
        for(int i=0;i<pair.second.second.size();i++){
            EV << pair.second.second.at(i) << "| ";
        }EV << std::endl;
    }EV << "-----------------------------" << endl;
}



void QoSMap::allocateAppsToQfi(vector<pair<int,double>> apps, string qfiClass, int PER,int pcp){
    int appsSize = apps.size();
    int classSize;
    int minAppsPerQfi;
    int remainder;
    int index = 0;  // Índice para o vetor de instâncias

    vector<int> classes;
    for (const auto& QFI : *QFI_table[qfiClass]){
        if (mapQoS.find(QFI.at("FiveQI")) == mapQoS.end()) {
            if(QFI.at("PER")>=PER){
                classes.push_back(QFI.at("FiveQI"));
            }
        }
    }
    EV << "Apps: " << apps.size() << endl;
    EV << "Classes disponiveis: " << classes.size() << endl;

    classSize = classes.size();
    minAppsPerQfi = appsSize/classSize;
    remainder = appsSize%classSize;

    sort(apps.begin(), apps.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;  // Comparando pelo segundo valor
    });

    if(classSize>appsSize){
        for(int i=0;i<appsSize;i++){
            vector<int> appV;
            appV.push_back(apps.at(i).first);
            mapQoS[classes.at(i)] = make_pair(pcp,appV);
        }
    }
    else{
        for (int i = 0; i < classSize; i++) {
            int count = minAppsPerQfi + (i < remainder ? 1 : 0);  // Adiciona uma instância extra se estiver no restante

            vector<int> appV;
            for (int j = 0; j < count; j++) {
                appV.push_back(apps.at(index++).first);
            }
            mapQoS[classes.at(i)] = make_pair(pcp,appV);
        }
    }
}


void QoSMap::allocateAppsToQfi2(vector<pair<int,double>> apps, int pcp,int iQfi,int fQfi){
    vector<int> classes;
    int appsSize = apps.size();
    int classSize;
    int minAppsPerQfi;
    int remainder;
    int index = 0;  // Índice para o vetor de instâncias

    EV << "Apps para mapear: " << apps.size() << endl;
    EV << "Classes: ";
    for (int i = iQfi; i < iQfi+fQfi;i++){
        FiveQI Qfi = QFI_table["GBR"]->at(i);
        EV << Qfi.at("FiveQI");
        if (mapQoS.find(Qfi.at("FiveQI")) == mapQoS.end()) {
            classes.push_back(Qfi.at("FiveQI"));
            EV << "(" << double(Qfi.at("PDB"))/1000 << ")";
        }EV << " ";
    }EV << endl;
    classSize = fQfi;
    EV << "Classes disponiveis para mapear: " << classSize << endl;
    minAppsPerQfi = appsSize/classSize;
    remainder = appsSize%classSize;
    sort(apps.begin(), apps.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;  // Comparando pelo segundo valor
    });

    if(classSize>appsSize){
        for(int i=0;i<appsSize;i++){
            vector<int> appV;
            appV.push_back(apps.at(i).first);
            mapQoS[classes.at(i)] = make_pair(pcp,appV);
        }
    }

    else{
        for (int i = 0; i < classes.size(); i++) {
            int count = minAppsPerQfi + (i < remainder ? 1 : 0);  // Adiciona uma instância extra se estiver no restante

            vector<int> appV;
            for (int j = 0; j < count; j++) {
                appV.push_back(apps.at(index++).first);
            }
            mapQoS[classes.at(i)] = make_pair(pcp,appV);
        }
    }
}



void QoSMap::allocateAppsToQfi3(vector<pair<int,double>> apps, int pcp,int iQfi,int fQfi){
    vector<int> classes;
    int appsSize = apps.size();
    int classSize;
    int minAppsPerQfi;
    int remainder;
    int index = 0;  // Índice para o vetor de instâncias

    EV << "Apps para mapear: " << apps.size() << endl;
    EV << "indices: " << iQfi << " " << fQfi  << endl;
    EV << "Classes: ";
    for (int i = iQfi; i < iQfi+fQfi;i++){
        FiveQI Qfi = QFI_table["DCGBR"]->at(i);
        EV << Qfi.at("FiveQI");
        if (mapQoS.find(Qfi.at("FiveQI")) == mapQoS.end()) {
                classes.push_back(Qfi.at("FiveQI"));
                EV << "(" << double(Qfi.at("PDB"))/1000 << ")";
        }EV << " ";
    }EV << endl;
    classSize = fQfi;
    EV << "Classes disponiveis para mapear: " << classSize << endl;
    minAppsPerQfi = appsSize/classSize;
    remainder = appsSize%classSize;
    sort(apps.begin(), apps.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;  // Comparando pelo segundo valor
    });


    if(classSize>appsSize){
        for(int i=0;i<appsSize;i++){
            vector<int> appV;
            appV.push_back(apps.at(i).first);
            mapQoS[classes.at(i)] = make_pair(pcp,appV);
        }
    }

    else{
        for (int i = 0; i < classes.size(); i++) {
            int count = minAppsPerQfi + (i < remainder ? 1 : 0);  // Adiciona uma instância extra se estiver no restante

            vector<int> appV;
            for (int j = 0; j < count; j++) {
                appV.push_back(apps.at(index++).first);
            }
            mapQoS[classes.at(i)] = make_pair(pcp,appV);
        }
    }
}


