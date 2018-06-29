

#include "../q4sCommon/ConfigFile.h"
#include <iostream>
using namespace std;
class Q4SServerConfigFile
{
public:

    Q4SServerConfigFile(const std::string &fName);

    string defaultTCPPort;
    string defaultUDPPort;
    string agentIP;
    string agentPort;
    bool isReactive;
    unsigned long alertPause;
    unsigned long recoveryPause;
    float latency;
    float jitterUp;
    float jitterDown;
    unsigned long bandwidthUp[11];
    unsigned long bandwidthDown[11];
    float packetLossUp;
    float packetLossDown;
    unsigned long timeBetweenPingsNegotiationUp;
    unsigned long timeBetweenPingsNegotiationDown;
    unsigned long timeBetweenPingsContinuityUp;
    unsigned long timeBetweenPingsContinuityDown;
    unsigned long bandwidthTime;
    unsigned long numberOfPingsUp;
    unsigned long numberOfPingsDown;
    unsigned long numberOfBwidthsUp;
    unsigned long numberOfBwidthsDown;
    unsigned long timeEndApp;
    bool showMeasureInfo;
    bool showReceivedPingInfo;
    bool showSocketReceivedInfo;
};

extern Q4SServerConfigFile q4SServerConfigFile;

