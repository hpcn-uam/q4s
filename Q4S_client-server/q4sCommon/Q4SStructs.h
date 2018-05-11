
#ifndef _Q4SSTRUCT_H_
#define _Q4SSTRUCT_H_

struct Q4SMeasurementValues
{
    float latency;
    float jitter;
    float bandwidth;
    float packetLoss;
    bool operator ==(const Q4SMeasurementValues compare) const;
};

class Q4SMeasurementResult
{
public:
    Q4SMeasurementResult() {latencyAlert = false; jitterAlert = false; bandwidthAlert = false; packetLossAlert = false;}
    ~Q4SMeasurementResult() {}
    Q4SMeasurementValues values;
    bool latencyAlert;
    bool jitterAlert;
    bool bandwidthAlert;
    bool packetLossAlert;
    bool operator ==(const Q4SMeasurementResult compare) const;
};

#endif //_Q4SSTRUCT_H_