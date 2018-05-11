#ifndef _Q4SSDPPARAMS_H_
#define _Q4SSDPPARAMS_H_

enum Q4SSDPAlertingMode
{ Q4SSDPALERTINGMODE_INVALID
, Q4SSDPALERTINGMODE_Q4SAWARENETWORK
, Q4SSDPALERTINGMODE_REACTIVE
};

struct Q4SSDPProcedure
{
    unsigned long negotiationTimeBetweenPingsUplink;
    unsigned long negotiationTimeBetweenPingsDownlink;
    unsigned long continuityTimeBetweenPingsUplink;
    unsigned long continuityTimeBetweenPingsDownlink;
    unsigned long bandwidthTime;
    unsigned long windowSizeLatencyCalcUplink;
    unsigned long windowSizeLatencyCalcDownlink;
    unsigned long windowSizePacketLossCalcUplink;
    unsigned long windowSizePacketLossCalcDownlink;

	bool operator ==(const Q4SSDPProcedure p) const;
};

struct Q4SSDPMeasurement
{
    Q4SSDPProcedure procedure;
    float latency;
    float jitter;
    float bandwidth;
    float packetLoss;
};

struct Q4SSDPParams
{
    int qosLevelUp; 
    int qosLevelDown; 
    Q4SSDPAlertingMode q4SSDPAlertingMode; 
    unsigned long alertPause;
    unsigned long recoveryPause;
    unsigned long latency;
    unsigned long jitterUp;
    unsigned long jitterDown;
    unsigned long bandWidthUp;
    unsigned long bandWidthDown;
    float packetLossUp;
    float packetLossDown;
	Q4SSDPProcedure procedure;
	
	bool operator ==(const Q4SSDPParams p) const;
};
#endif  // _Q4SSDPPARAMS_H_