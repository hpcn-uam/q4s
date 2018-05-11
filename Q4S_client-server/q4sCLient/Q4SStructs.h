struct Q4SMeasurementValues
{
    float latency;
    float jitter;
    float bandWith;
    float packetLoss;
};

class Q4SMeasurementResult
{
public:
    Q4SMeasurementResult() {latency = false; jitter = false; bandWith = false; packetLoss = false;}
    ~Q4SMeasurementResult() {}
    Q4SMeasurementValues values;
    bool latency;
    bool jitter;
    bool bandWith;
    bool packetLoss;
};

struct Q4SMeasurementStage0Limits
{
    float maxLatency;
    float maxJitter;
};

struct Q4SMeasurementStage1Limits
{
    float minBandWith;
    float maxPacketLoss;
};

struct Q4SMeasurementLimits
{
    Q4SMeasurementStage0Limits stage0;
    Q4SMeasurementStage1Limits stage1;
};