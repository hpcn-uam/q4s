#include "Q4SStructs.h"

bool Q4SMeasurementValues::operator ==(const Q4SMeasurementValues compare) const
{
	bool equal = true;
	equal &= (this->latency == compare.latency);
	equal &= (this->jitter == compare.jitter);
	equal &= (this->bandwidth == compare.bandwidth);
	equal &= (this->packetLoss == compare.packetLoss);
	return equal;
}

bool Q4SMeasurementResult::operator ==(const Q4SMeasurementResult compare) const
{
	bool equal = true;
	equal &= (this->latencyAlert == compare.latencyAlert);
	equal &= (this->jitterAlert == compare.jitterAlert);
	equal &= (this->bandwidthAlert == compare.bandwidthAlert);
	equal &= (this->packetLossAlert == compare.packetLossAlert);
	equal &= (this->values.latency == compare.values.latency);
	equal &= (this->values.jitter == compare.values.jitter);
	equal &= (this->values.bandwidth == compare.values.bandwidth);
	equal &= (this->values.packetLoss == compare.values.packetLoss);
	return equal;
}