
#ifndef _Q4SAGENTCONFIGFILE_H_
#define _Q4SAGENTCONFIGFILE_H_

#include "../q4sCommon/ConfigFile.h"

class Q4SAgentConfigFile
{
public:

    Q4SAgentConfigFile(const std::string &fName);

    std::string ganyConnectorIp;
    std::string ganyConnectorPort;
    std::string listenUDPPort;
    unsigned long timeEndApp;
    unsigned long initialBitRate;
    unsigned long bitRateReduceCuantity;
    bool demoMode;
    bool demoConnSocket;
    bool showSocketReceivedInfo;
};

extern Q4SAgentConfigFile q4SAgentConfigFile;

#endif //_Q4SAGENTCONFIGFILE_H_