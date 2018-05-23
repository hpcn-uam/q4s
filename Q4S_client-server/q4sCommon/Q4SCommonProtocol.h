#ifndef _Q4SCOMMONPROTOCOL_H_
#define _Q4SCOMMONPROTOCOL_H_
#include <iostream>
#include "Q4SStructs.h"
#include <vector>
#include <set>
#include "Q4SSDPParams.h"
#include <algorithm> 

class Q4SMessageManager;

class Q4SCommonProtocol
{
    protected:
		bool 	LookforCancel(Q4SMessageManager &mReceivedMessages);

        void    calculateLatency(
									Q4SMessageManager &mReceivedMessages, 
									std::vector<unsigned long> &arrSentPingTimestamps, 
									float &latency, 
									unsigned long pingsSent, 
									bool showMeasureInfo=true);
        void    calculateJitterStage0(
									Q4SMessageManager &mReceivedMessages, 
									float &jitter, 
									unsigned long timeBetweenPings, 
									unsigned long pingsSent, 
									bool showMeasureInfo=true);
        void    calculateJitterAndPacketLossContinuity(
												Q4SMessageManager &mReceivedMessages, 
												float &jitter, 
												unsigned long timeBetweenPings, 
												unsigned long pingsSent, 
												float &packetLoss, 
												bool showMeasureInfo=true);
        bool    calculateBandwidthPacketLossStage1(Q4SMessageManager &mReceivedMessages, float &packetLoss, unsigned long bandwidthTime, float &bandwidth);
        bool    checkStage0(unsigned long maxLatencyUp, unsigned long maxJitterUp, 
			unsigned long maxLatencyDown, unsigned long maxJitterDown, 
			Q4SMeasurementResult &upResults, 
			Q4SMeasurementResult &downResults);
		bool	checkStage1(
			unsigned long bandwidthUp, float packetLossUp, 
			unsigned long bandwidthDown, float packetLossDown, 
			Q4SMeasurementResult &upResults, 
			Q4SMeasurementResult &downResults);
		bool	checkContinuity(
			unsigned long maxLatencyUp, unsigned long maxJitterUp, float maxPacketLossUp, 
			unsigned long maxLatencyDown, unsigned long maxJitterDown, float maxPacketLossDown, 
			Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults);
		void showCheckMessage(Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults);
		std::string generateAlertMessage(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults);
		std::string generateNotificationAlertMessage(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults);
		
	private:

        bool    checkStage0(unsigned long maxLatency, unsigned long maxJitter, Q4SMeasurementResult &results);
		bool	checkStage1(unsigned long bandwidth, float packetLoss, Q4SMeasurementResult &results);
		bool	checkContinuity(unsigned long maxLatency, unsigned long maxJitter, float maxPacketLoss, Q4SMeasurementResult &results);
        void    calculateJitter(
								Q4SMessageManager &mReceivedMessages, 
								float &jitter, 
								unsigned long timeBetweenPings, 
								unsigned long pingsSent, 
								bool calculatePacketLoss, 
								float &packetLoss, 
								bool showMeasureInfo);
		std::set<unsigned long> obtainSortedSequenceNumberList(Q4SMessageManager &mReceivedMessages, std::set<unsigned long> &TimeStamplist);
		int contador= 0; 
	/*
	// TEST
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityTwoMessagesWithoutJitter);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityTwoMessagesWithJitter1);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityTwoMessagesWithJitter2);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityThreeMessagesWithoutJitter);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityThreeMessagesWithPacketLoss);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityFourMessagesWithPacketLossTwoGaps);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityFourMessagesWithPacketLossTwoSpaces);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityThreeMessagesWithJitter1);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityThreeMessagesWithJitter2);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCalculateLatencyThreeMessages);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCalculateLatencyTwoMessages);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCalculateLatencyOneMessage);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCalculateLatencyOneMessageLost);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1OneMessageBegin);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1OneMessageBWidth);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1TwoMessageBWidthWithoutPacketLoss);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1TwoMessageBWidthWithPacketLoss);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1TwoMessageBWidthWithPacketLossTwoGaps);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1ThreeMessageBWidthWithPacketLoss);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1FourMessageBWidthWithPacketLoss);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1ThreeMessageBWidthWithPacketLossTwoSpaces);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage0BothAlert);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage0LatencyAlert);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage0JitterAlert);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage0NoAlert);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage1BothAlert);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage1BandwidthAlert);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage1PacketLossAlert);
		FRIEND_TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage1NoAlert);
		*/
};

#endif // _Q4SCOMMONPROTOCOL_H_