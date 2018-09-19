#include "Q4SSDPParams.h"

bool Q4SSDPProcedure::operator ==(const Q4SSDPProcedure p) const
{
	bool equal = true;
	equal &= (this->negotiationTimeBetweenPingsUplink == p.negotiationTimeBetweenPingsUplink);
	equal &= (this->negotiationTimeBetweenPingsDownlink == p.negotiationTimeBetweenPingsDownlink);
	equal &= (this->continuityTimeBetweenPingsUplink == p.continuityTimeBetweenPingsUplink);
	equal &= (this->continuityTimeBetweenPingsDownlink == p.continuityTimeBetweenPingsDownlink);
	equal &= (this->bandwidthTime == p.bandwidthTime);
	equal &= (this->windowSizeLatencyCalcUplink == p.windowSizeLatencyCalcUplink);
	equal &= (this->windowSizeLatencyCalcDownlink == p.windowSizeLatencyCalcDownlink);
	equal &= (this->windowSizePacketLossCalcUplink == p.windowSizePacketLossCalcUplink);
	equal &= (this->windowSizePacketLossCalcDownlink == p.windowSizePacketLossCalcDownlink);
	
	return equal;
}

bool Q4SSDPParams::operator ==(const Q4SSDPParams p) const
{
	bool equal = true;
	equal &= (this->qosLevelUp == p.qosLevelUp);
	equal &= (this->qosLevelDown == p.qosLevelDown);
	equal &= (this->q4SSDPAlertingMode == p.q4SSDPAlertingMode);
	equal &= (this->alertPause == p.alertPause);
	equal &= (this->recoveryPause == p.recoveryPause);
	equal &= (this->latency == p.latency);
	equal &= (this->jitterUp == p.jitterUp);
	equal &= (this->jitterDown == p.jitterDown);
	equal &= (this->bandWidthUp == p.bandWidthUp);
	equal &= (this->bandWidthDown == p.bandWidthDown);
	equal &= (this->packetLossUp == p.packetLossUp);
	equal &= (this->packetLossDown == p.packetLossDown);
	equal &= (this->ready_BW == p.ready_BW);
	equal &= (this->procedure == p.procedure);
	
	return equal;
}