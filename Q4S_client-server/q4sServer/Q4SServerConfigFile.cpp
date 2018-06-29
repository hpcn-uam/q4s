#include "Q4SServerConfigFile.h"

Q4SServerConfigFile q4SServerConfigFile("Q4SServerConfig.txt");

Q4SServerConfigFile::Q4SServerConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    defaultTCPPort = configFile.getValueOfKey<std::string>("DEFAULT_TCP_PORT", "27015");
    defaultUDPPort = configFile.getValueOfKey<std::string>("DEFAULT_UDP_PORT", "27016");
    agentIP = configFile.getValueOfKey<std::string>("AGENT_IP", "127.0.0.1");
    agentPort = configFile.getValueOfKey<std::string>("AGENT_PORT", "27017");
    isReactive = configFile.keyExists("IS_REACTIVE");
    alertPause = configFile.getValueOfKey<unsigned long>("ALERT_PAUSE", 2000);
    recoveryPause = configFile.getValueOfKey<unsigned long>("RECOVERY_PAUSE", 4000);
    latency = configFile.getValueOfKey<float>("LATENCY", 3);
    jitterUp = configFile.getValueOfKey<float>("JITTER_UP", 200);
    jitterDown = configFile.getValueOfKey<float>("JITTER_DOWN", 200);
    bandwidthUp[0] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_0", 1000);
    bandwidthDown[0] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_0", 1000);    
    bandwidthUp[1] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_1", 900);
    bandwidthDown[1] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_1", 900);
    bandwidthUp[2] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_2", 800);
    bandwidthDown[2] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_2", 800);    
    bandwidthUp[3] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_3", 750);
    bandwidthDown[3] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_3", 750);
    bandwidthUp[4] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_4", 700);
    bandwidthDown[4] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_4", 700);    
    bandwidthUp[5] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_5", 650);
    bandwidthDown[5] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_5", 650);
    bandwidthUp[6] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_6", 600);
    bandwidthDown[6] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_6", 600);    
    bandwidthUp[7] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_7", 550);
    bandwidthDown[7] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_7", 550);
    bandwidthUp[8] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_8", 500);
    bandwidthDown[8] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_8", 500);    
    bandwidthUp[9] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_9", 450);
    bandwidthDown[9] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_9", 450);    
    bandwidthUp[10] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP_10", 400);
    bandwidthDown[10] = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN_10", 400);
    packetLossUp = configFile.getValueOfKey<float>("PACKETLOSS_UP", 1.00f);
    packetLossDown = configFile.getValueOfKey<float>("PACKETLOSS_UP", 1.00f);
    timeBetweenPingsNegotiationUp = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS_NEGOTITATION_UP", 20);
    timeBetweenPingsNegotiationDown = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS_NEGOTITATION_DOWN", 20);
    timeBetweenPingsContinuityUp = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS_CONTINUITY_UP", 20);
    timeBetweenPingsContinuityDown = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS_CONTINUITY_DOWN", 20);
    bandwidthTime= configFile.getValueOfKey<unsigned long>("BANDWIDTH_TIME", 2000);
    numberOfPingsUp = configFile.getValueOfKey<unsigned long>("NUMBER_OF_PINGS_UP", 20);
    numberOfPingsDown = configFile.getValueOfKey<unsigned long>("NUMBER_OF_PINGS_DOWN", 20);
    numberOfBwidthsUp = configFile.getValueOfKey<unsigned long>("NUMBER_OF_BWIDTHS_UP", 20);
    numberOfBwidthsDown = configFile.getValueOfKey<unsigned long>("NUMBER_OF_BWIDTHS_DOWN", 20);
    timeEndApp = configFile.getValueOfKey<unsigned long>("TIME_END_APP", 10000);
    showMeasureInfo = configFile.keyExists("SHOW_MEASURE_INFO");
    showReceivedPingInfo = configFile.keyExists("SHOW_RECEIVED_PING_INFO");
    showSocketReceivedInfo = configFile.keyExists("SHOW_SOCKET_RECEIVED_INFO");
}