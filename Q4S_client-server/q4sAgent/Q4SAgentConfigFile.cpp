#include "Q4SAgentConfigFile.h"

Q4SAgentConfigFile q4SAgentConfigFile("Q4SAgentConfig.txt");

Q4SAgentConfigFile::Q4SAgentConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    ganyConnectorIp = configFile.getValueOfKey<std::string>("GANY_CONNECTOR_IP", "127.0.0.1");
    ganyConnectorPort = configFile.getValueOfKey<std::string>("GANY_CONNECTOR_PORT", "27015");
    listenUDPPort = configFile.getValueOfKey<std::string>("LISTEN_UDP_PORT", "27017");
    timeEndApp = configFile.getValueOfKey<unsigned long>("TIME_END_APP", 10000);
    initialBitRate = configFile.getValueOfKey<unsigned long>("INITIAL_BITRATE", 5000);
    bitRateReduceCuantity = configFile.getValueOfKey<unsigned long>("BITRATE_REDUCE_CUANTITY", 500);
    demoMode= configFile.keyExists("DEMO_MODE");
    demoConnSocket= configFile.keyExists("DEMO_CONN_SOCKET");    
    showSocketReceivedInfo = configFile.keyExists("SHOW_SOCKET_RECEIVED_INFO");

}    
