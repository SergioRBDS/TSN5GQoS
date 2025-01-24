/*
 * QoSMap.h
 *
 *  Created on: Jul 14, 2024
 *      Author: pregui
 */

#ifndef SRC_QOSMAP_H_
#define SRC_QOSMAP_H_

#include<omnetpp.h>
#include<map>
#include<string>
#include<vector>
#include <inet/networklayer/common/L3Address.h>

using namespace std;
using namespace omnetpp;


typedef map<string, int> FiveQI;

typedef struct{
    int pktLength;
    double intervall; // nanosseconds
}App;

class QoSMap : public cSimpleModule {
    public:
        //map<int,vector<App*>> *appsByPCP;

        cValueMap* Apps;
        map<int,pair<int,vector<int>>> mapQoS;
        vector<int>* BE;
        vector<int>* AVB;
        vector<int>* ST;

        cValueArray* ctrlParams;

        virtual void finish() override {
            //delete Apps;

        }



        int getPcpFromQfi(int i);
        int getQfiFromPcp(int i);
        void printMap();
    protected:
        virtual void initialize(int stages) override;
        map<string,vector<FiveQI>*> QFI_table;

        void mappingQoS();


    private:
        vector<int>* readPCPs(string AppType);

        vector<string> FGFlowType = {"GBR", "NonGBR", "DCGBR"};
        void read5QIFile(cXMLElement* root);
        void readTSNAppsFile(cXMLElement* root);
        void mappingNonGBR(vector<FiveQI>* GBR);
        void mappingGBR(vector<FiveQI>* NGBR);
        void mappingDCGBR(vector<FiveQI>* DCGBR);

        void allocateAppsToQfi(vector<pair<int,double>> apps, string qfiClass, int PER,int pcp);
        void allocateAppsToQfi2(vector<pair<int,double>> apps, int pcp,int iQfi,int fQfi);
        void allocateAppsToQfi3(vector<pair<int,double>> apps, int pcp,int iQfi,int fQfi);


};



#endif /* SRC_QOSMAP_H_ */
