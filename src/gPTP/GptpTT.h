/*
 * GptpTT.h
 *
 *  Created on: Nov 21, 2024
 *      Author: sergio
 */

#ifndef __GPTPTT_H_
#define __GPTPTT_H_

#include "inet/clock/contract/ClockTime.h"
#include "inet/clock/common/ClockTime.h"
#include "inet/clock/model/SettableClock.h"
#include "inet/common/INETDefs.h"
#include "inet/common/ModuleRefByPar.h"
#include "inet/common/clock/ClockUserModuleBase.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/linklayer/ieee8021as/GptpPacket_m.h"

using namespace std;
using namespace omnetpp;
using namespace inet;


class GptpTT : public ClockUserModuleBase, public cListener
{

protected:

    int domainNumber = -1;
    uint64_t clockIdentity = 0;

    ModuleRefByPar<IInterfaceTable> interfaceTable;

    int fiveGPortId;
    int tsnPortId;
    int slavePortId = -1; // interface ID of slave port
    set<int> masterPortIds; // interface IDs of master ports


    clocktime_t pdelayInterval = 0;
    clocktime_t pDelayReqProcessingTime = 0;

    //peerdelay
    clocktime_t peerRequestReceiptTimestamp;  // T2
    clocktime_t peerResponseOriginTimestamp; // T3
    clocktime_t pdelayRespEventIngressTimestamp;  // T4
    clocktime_t pdelayReqEventEgressTimestamp;   // T1

    clocktime_t originTimestamp; // tempo q saiu mensagem sync
    clocktime_t syncIngressTimestamp; // tempo q a mensagem sync entrou no 5g
    clocktime_t sentTimeSyncSync; // tempo q a mensagem sync saiu do 5g

    double gmRateRatio = 1.0;
    double receivedRateRatio = 1.0;
    clocktime_t peerDelay;
    clocktime_t correctionField;
    clocktime_t suffValue = 0;


    uint16_t sequenceIdPeerDelay = 0;
    uint16_t lastSentPdelayReqSequenceId = 0;
    uint16_t lastReceivedGptpSyncSequenceId = 0xffff;
    uint16_t sequenceIdSync = 0;
    bool rcvdPdelayResp = false;
    bool rcvdGptpSync = false;
    bool rcvdGptpSyncFollowUp = false;

    static const MacAddress GPTP_MULTICAST_ADDRESS;

    ClockEvent* selfMsgDelayReq = nullptr;
    ClockEvent* requestMsg = nullptr;

    static simsignal_t peerDelaySignal;
    static simsignal_t sequenceIDSignal;
    static simsignal_t rateRatioSignal;

    clocktime_t receivedTimeSync;
    clocktime_t peerSentTimeSync;  // sending time of last received GptpSync
    clocktime_t oldPeerSentTimeSync = -1;  // sending time of previous received GptpSync



    virtual ~GptpTT();
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void handleSelfMessage(cMessage *msg);

    void sendPdelayReq();
    void processPdelayReq(Packet *packet, const GptpPdelayReq* gptp);

    void sendPdelayResp(GptpReqAnswerEvent *req);
    void sendPdelayRespFollowUp(int portId, const GptpPdelayResp* resp, clocktime_t preciseOriginTimestamp);

    void processPdelayResp(Packet *packet, const GptpPdelayResp* gptp);
    void processPdelayRespFollowUp(Packet *packet, const GptpPdelayRespFollowUp* gptp);

    void processSyncTSN(Packet *packet, const GptpSync* gptp);
    void processSync5G(Packet *packet, const GptpSync* gptp);
    void processFollowUpTSN(Packet *packet, const GptpFollowUp* gptp);
    void processFollowUp5G(Packet *packet, const GptpFollowUp* gptp);
    void sendFollowUp(int portId, clocktime_t preciseOriginTimestamp);

    void sendPacketToNIC(Packet *packet, int portId);

    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *obj, cObject *details) override;

    void calculatePeerDelay();
};

#endif /* SRC_GPTP_GPTPTT_H_ */
