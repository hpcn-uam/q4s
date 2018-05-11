
#ifndef _Q4SSDP_H_
#define _Q4SSDP_H_
#include <string> 

#include "Q4SSDPParams.h"

#define QOSLEVEL_PATTERN "a=qos-level:"
#define ALERTINGMODE_PATTERN "a=alerting-mode:"
#define ALERTPAUSE_PATTERN "a=alert-pause:"
#define RECOVERYPAUSE_PATTERN "a=recovery-pause:"
#define APPLATENCY_PATTERN "a=application:latency:"
#define APPJITTER_PATTERN "a=application:jitter:"
#define APPBANDWIDTH_PATTERN "a=application:bandwidth:"
#define APPPACKETLOSS_PATTERN "a=application:packetloss:"
#define PROCEDURE_PATTERN "a=measurement:procedure default("
#define PROCEDURE_CLOSE_PATTERN ")"

#define Q4SSDPALERTINGMODE_Q4SAWARENETWORK_PATTERN "Q4S-aware-network"
#define Q4SSDPALERTINGMODE_REACTIVE_PATTERN "Reactive"

std::string Q4SSDP_create(Q4SSDPParams q4SSDPParams);
bool Q4SSDP_parse(std::string message, Q4SSDPParams& params);

#endif  // _Q4SSDP_H_