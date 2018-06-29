#include "Q4SSDP.h"

#include <sstream>

Q4SSDPAlertingMode Q4SSDP_alertingMode_getFromText(std::string alertingModeText)
{
    Q4SSDPAlertingMode alertingMode = Q4SSDPALERTINGMODE_INVALID;

    if (alertingModeText.compare(Q4SSDPALERTINGMODE_Q4SAWARENETWORK_PATTERN) == 0)
    {
        alertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
    }
    else
    {
        if (alertingModeText.compare(Q4SSDPALERTINGMODE_REACTIVE_PATTERN) == 0)
        {
            alertingMode = Q4SSDPALERTINGMODE_REACTIVE;
        }
    }

    return alertingMode;
}

std::string Q4SSDP_alertingMode_getText(Q4SSDPAlertingMode alertingMode)
{
    std::string text;

    switch (alertingMode)
    {
        case Q4SSDPALERTINGMODE_Q4SAWARENETWORK:
            {
                text.append(Q4SSDPALERTINGMODE_Q4SAWARENETWORK_PATTERN);
            }
        break;

        case Q4SSDPALERTINGMODE_REACTIVE:
            {
                text.append(Q4SSDPALERTINGMODE_REACTIVE_PATTERN);
            }
        break;
    }

    return text;
}

std::string makeQosLevelLine(int up, int down)
{
    std::string line;
    line.append(QOSLEVEL_PATTERN);
    line.append(std::to_string(( unsigned long long int)up));
    line.append("/");
    line.append(std::to_string(( unsigned long long int)down));
    line.append("\n");
    return line;
}

std::string makeAlertingModeLine(Q4SSDPAlertingMode alertingMode)
{
    std::string line;
    line.append(ALERTINGMODE_PATTERN);
    line.append(Q4SSDP_alertingMode_getText(alertingMode));
    line.append("\n");
    return line;
}

std::string makeAppAlertPauseLine(unsigned long appAlertPause)
{
    std::string line;
    line.append(ALERTPAUSE_PATTERN);
    line.append(std::to_string(( unsigned long long int)appAlertPause));
    line.append("\n");
    return line;
}

std::string makeRecoveryPauseLine(unsigned long recoveryPause)
{
    std::string line;
    line.append(RECOVERYPAUSE_PATTERN);
    line.append(std::to_string(( unsigned long long int)recoveryPause));
    line.append("\n");
    return line;
}

std::string makeLatencyLine(float latency)
{
    std::string line;
    line.append(APPLATENCY_PATTERN);
    line.append(std::to_string(( float)latency));
    line.append("\n");
    return line;
}

std::string makeJitterLine(float jitterUp, float jitterDown)
{
    std::string line;
    line.append(APPJITTER_PATTERN);
    line.append(std::to_string((float)jitterUp));
    line.append("/");
    line.append(std::to_string((float)jitterDown));
    line.append("\n");
    return line;
}

std::string makeBandWidthLine(unsigned long bandwidthUp, unsigned long bandwidthDown)
{
    std::string line;
    line.append(APPBANDWIDTH_PATTERN);
    line.append(std::to_string(( unsigned long long int)bandwidthUp));
    line.append("/");
    line.append(std::to_string(( unsigned long long int)bandwidthDown));
    line.append("\n");
    return line;
}

std::string makePacketLossLine(float packetLossUp, float packetLossDown)
{
    std::string line;
    line.append(APPPACKETLOSS_PATTERN);
    std::ostringstream streamPacketLossUp;
    streamPacketLossUp << packetLossUp;
    line.append(streamPacketLossUp.str());
    line.append("/");
    std::ostringstream streamPacketLossDown;
    streamPacketLossDown << packetLossUp;
    line.append(streamPacketLossUp.str());
    line.append("\n");
    return line;
}

std::string makeProcedureLine(Q4SSDPProcedure procedure)
{
    std::string line;
    line.append(PROCEDURE_PATTERN);
    line.append(std::to_string(( unsigned long long int)procedure.negotiationTimeBetweenPingsUplink));
    line.append("/");
    line.append(std::to_string(( unsigned long long int)procedure.negotiationTimeBetweenPingsDownlink));
    line.append(",");
    line.append(std::to_string(( unsigned long long int)procedure.continuityTimeBetweenPingsUplink));
    line.append("/");
    line.append(std::to_string(( unsigned long long int)procedure.continuityTimeBetweenPingsDownlink));
    line.append(",");
    line.append(std::to_string(( unsigned long long int)procedure.bandwidthTime));
    line.append(",");
    line.append(std::to_string(( unsigned long long int)procedure.windowSizeLatencyCalcUplink));
    line.append("/");
    line.append(std::to_string(( unsigned long long int)procedure.windowSizeLatencyCalcDownlink));
    line.append(",");
    line.append(std::to_string(( unsigned long long int)procedure.windowSizePacketLossCalcUplink));
    line.append("/");
    line.append(std::to_string(( unsigned long long int)procedure.windowSizePacketLossCalcDownlink));
    line.append(PROCEDURE_CLOSE_PATTERN);
    line.append("\n");
    return line;
}

bool Q4SSDP_parseOneElementLine(std::string message, std::string pattern, std::string &paramText)
{
    bool ok = true;

    std::string::size_type initialPosition;
    std::string::size_type finalPosition;

    if (ok)
    {   
        initialPosition = message.find(pattern) + pattern.length();
        if (initialPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        finalPosition = message.find("\n", initialPosition+1);
        if (finalPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        paramText = message.substr(initialPosition, finalPosition-initialPosition);
    }
    
    return ok;
}

bool Q4SSDP_parseTwoElementsLine(std::string message, std::string pattern, std::string separator, std::string &firstParamText, std::string &secondParamText)
{
    bool ok = true;

    std::string::size_type initialPosition;
    std::string::size_type finalPosition;
    std::string::size_type betweenPosition;

    if (ok)
    {   
        initialPosition = message.find(pattern) + pattern.length();
        if (initialPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenPosition = message.find(separator, initialPosition+1);
        if (betweenPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        finalPosition = message.find("\n", betweenPosition+1);
        if (finalPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        firstParamText = message.substr(initialPosition, betweenPosition-initialPosition);
        secondParamText = message.substr(betweenPosition + separator.length(), finalPosition - (betweenPosition + separator.length()));
    }
    
    return ok;
}

bool Q4SSDP_parseQosLevel(std::string message, int& up, int& down)
{
    bool ok = true;

    std::string pattern = QOSLEVEL_PATTERN;
    std::string firstParamText;
    std::string secondParamText;

    ok &= Q4SSDP_parseTwoElementsLine(message, pattern, "/", firstParamText, secondParamText);

    if (ok)
    {
        up = std::stoi(firstParamText);
        down = std::stoi(secondParamText);
    }

    return ok;
}

bool Q4SSDP_parseAlertingMode(std::string message, Q4SSDPAlertingMode& alertingMode)
{
    bool ok = true;

    std::string pattern = ALERTINGMODE_PATTERN;
    std::string paramText;

    ok &= Q4SSDP_parseOneElementLine (message, pattern, paramText); 

    if (ok)
    {
        alertingMode = Q4SSDP_alertingMode_getFromText(paramText);
    }
    
    return ok;
}

bool Q4SSDP_parseAlertPause(std::string message, unsigned long& appAlertPause)
{
    bool ok = true;

    std::string pattern = ALERTPAUSE_PATTERN;
    std::string paramText;

    ok &= Q4SSDP_parseOneElementLine (message, pattern, paramText); 

    if (ok)
    {
        appAlertPause = std::stol(paramText);
    }
    
    return ok;
}

bool Q4SSDP_parseRecoveryPause(std::string message, unsigned long& recoveryPause)
{
    bool ok = true;

    std::string pattern = RECOVERYPAUSE_PATTERN;
    std::string paramText;

    ok &= Q4SSDP_parseOneElementLine (message, pattern, paramText); 

    if (ok)
    {
        recoveryPause = std::stol(paramText);
    }
    
    return ok;
}

bool Q4SSDP_parseLatency(std::string message, float& latency)
{
    bool ok = true;

    std::string pattern = APPLATENCY_PATTERN;
    std::string paramText;

    ok &= Q4SSDP_parseOneElementLine (message, pattern, paramText); 

    if (ok)
    {
        latency = std::stof(paramText);
    }
    
    return ok;
}

bool Q4SSDP_parseJitter(std::string message, float& up, float& down)
{
    bool ok = true;

    std::string pattern = APPJITTER_PATTERN;
    std::string firstParamText;
    std::string secondParamText;

    ok &= Q4SSDP_parseTwoElementsLine(message, pattern, "/", firstParamText, secondParamText);

    if (ok)
    {
        up = std::stof(firstParamText);
        down = std::stof(secondParamText);
    }

    return ok;
}

bool Q4SSDP_parseBandWidth(std::string message, unsigned long& up, unsigned long& down)
{
    bool ok = true;

    std::string pattern = APPBANDWIDTH_PATTERN;
    std::string firstParamText;
    std::string secondParamText;

    ok &= Q4SSDP_parseTwoElementsLine(message, pattern, "/", firstParamText, secondParamText);

    if (ok)
    {
        up = std::stol(firstParamText);
        down = std::stol(secondParamText);
    }

    return ok;
}

bool Q4SSDP_parsePacketLoss(std::string message, float& up, float& down)
{
    bool ok = true;

    std::string pattern = APPPACKETLOSS_PATTERN;
    std::string firstParamText;
    std::string secondParamText;

    ok &= Q4SSDP_parseTwoElementsLine(message, pattern, "/", firstParamText, secondParamText);

    if (ok)
    {
        up = std::stof(firstParamText);
        down = std::stof(secondParamText);
    }

    return ok;
}

bool Q4SSDP_parseProcedure(std::string message, Q4SSDPProcedure &procedure)
{
    bool ok = true;

    std::string::size_type initialNegotiationTimeBetweenPingsUplinkPosition;
    std::string::size_type betweenNegotiationTimeBetweenPingsPosition;
    std::string::size_type betweenNegotiationContinuityPosition;
    std::string::size_type betweenContinuityTimeBetweenPingsPosition;
    std::string::size_type betweenContinuityBandwidthPosition;
    std::string::size_type betweenBandwidthWindowSizeLatencyCalcUplinkPosition;
    std::string::size_type betweenWindowSizeLatencysPosition;
    std::string::size_type betweenWindowSizeLatencysWindowSizePacketLossesPosition;
    std::string::size_type betweenWindowSizePacketLossesPosition;
    std::string::size_type finalPosition;

    std::string pattern = PROCEDURE_PATTERN;
    if (ok)
    {   
        initialNegotiationTimeBetweenPingsUplinkPosition = message.find(pattern) + pattern.length();
        if (initialNegotiationTimeBetweenPingsUplinkPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenNegotiationTimeBetweenPingsPosition = message.find("/", initialNegotiationTimeBetweenPingsUplinkPosition+1);
        if (betweenNegotiationTimeBetweenPingsPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenNegotiationContinuityPosition = message.find(",", betweenNegotiationTimeBetweenPingsPosition+1);
        if (betweenNegotiationContinuityPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenContinuityTimeBetweenPingsPosition = message.find("/", betweenNegotiationContinuityPosition+1);
        if (betweenContinuityTimeBetweenPingsPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenContinuityBandwidthPosition = message.find(",", betweenContinuityTimeBetweenPingsPosition+1);
        if (betweenContinuityBandwidthPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenBandwidthWindowSizeLatencyCalcUplinkPosition = message.find(",", betweenContinuityBandwidthPosition+1);
        if (betweenBandwidthWindowSizeLatencyCalcUplinkPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenWindowSizeLatencysPosition = message.find("/", betweenBandwidthWindowSizeLatencyCalcUplinkPosition+1);
        if (betweenWindowSizeLatencysPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenWindowSizeLatencysWindowSizePacketLossesPosition = message.find(",", betweenWindowSizeLatencysPosition+1);
        if (betweenWindowSizeLatencysWindowSizePacketLossesPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        betweenWindowSizePacketLossesPosition = message.find("/", betweenWindowSizeLatencysWindowSizePacketLossesPosition+1);
        if (betweenWindowSizePacketLossesPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        finalPosition = message.find(PROCEDURE_CLOSE_PATTERN, betweenWindowSizePacketLossesPosition);
        if (finalPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        procedure.negotiationTimeBetweenPingsUplink = std::stol(message.substr(initialNegotiationTimeBetweenPingsUplinkPosition, betweenNegotiationTimeBetweenPingsPosition - initialNegotiationTimeBetweenPingsUplinkPosition));
        procedure.negotiationTimeBetweenPingsDownlink = std::stol(message.substr(betweenNegotiationTimeBetweenPingsPosition + 1, betweenNegotiationContinuityPosition - (betweenNegotiationTimeBetweenPingsPosition + 1 )));
        procedure.continuityTimeBetweenPingsUplink = std::stol(message.substr(betweenNegotiationContinuityPosition + 1, betweenContinuityTimeBetweenPingsPosition - (betweenNegotiationContinuityPosition + 1 )));
        procedure.continuityTimeBetweenPingsDownlink = std::stol(message.substr(betweenContinuityTimeBetweenPingsPosition + 1, betweenContinuityBandwidthPosition - (betweenContinuityTimeBetweenPingsPosition + 1 )));
        procedure.bandwidthTime = std::stol(message.substr(betweenContinuityBandwidthPosition + 1, betweenBandwidthWindowSizeLatencyCalcUplinkPosition - (betweenContinuityBandwidthPosition + 1 )));
        procedure.windowSizeLatencyCalcUplink = std::stol(message.substr(betweenBandwidthWindowSizeLatencyCalcUplinkPosition + 1, betweenWindowSizeLatencysPosition - (betweenBandwidthWindowSizeLatencyCalcUplinkPosition + 1 )));
        procedure.windowSizeLatencyCalcDownlink = std::stol(message.substr(betweenWindowSizeLatencysPosition + 1, betweenWindowSizeLatencysWindowSizePacketLossesPosition - (betweenWindowSizeLatencysPosition + 1 )));
        procedure.windowSizePacketLossCalcUplink = std::stol(message.substr(betweenWindowSizeLatencysWindowSizePacketLossesPosition + 1, betweenWindowSizePacketLossesPosition - (betweenWindowSizeLatencysWindowSizePacketLossesPosition + 1 )));
        procedure.windowSizePacketLossCalcDownlink = std::stol(message.substr(betweenWindowSizePacketLossesPosition + 1, finalPosition - (betweenWindowSizePacketLossesPosition + 1 )));
    }
    
    return ok;
}

std::string Q4SSDP_create(Q4SSDPParams params)
{
    std::string message;
    message.append( makeQosLevelLine(params.qosLevelUp, params.qosLevelDown));
    message.append( makeAlertingModeLine(params.q4SSDPAlertingMode));
    message.append( makeAppAlertPauseLine(params.alertPause));
    message.append( makeRecoveryPauseLine(params.recoveryPause));
    message.append( makeLatencyLine(params.latency));
    message.append( makeJitterLine(params.jitterUp, params.jitterDown));
    message.append( makeBandWidthLine(params.bandWidthUp, params.bandWidthDown));
    message.append( makePacketLossLine(params.packetLossUp, params.packetLossDown));
    message.append( makeProcedureLine(params.procedure));
    
    return message;
}

bool Q4SSDP_parse(std::string message, Q4SSDPParams& params)
{
    bool ok = true;

    ok &= Q4SSDP_parseQosLevel(message, params.qosLevelUp, params.qosLevelDown);
    ok &= Q4SSDP_parseAlertingMode(message, params.q4SSDPAlertingMode);
    ok &= Q4SSDP_parseAlertPause(message, params.alertPause);
    ok &= Q4SSDP_parseRecoveryPause(message, params.recoveryPause);
    ok &= Q4SSDP_parseLatency(message, params.latency);
    ok &= Q4SSDP_parseJitter(message, params.jitterUp, params.jitterDown);
    ok &= Q4SSDP_parseBandWidth(message, params.bandWidthUp, params.bandWidthDown);
    ok &= Q4SSDP_parsePacketLoss(message, params.packetLossUp, params.packetLossDown);
    ok &= Q4SSDP_parseProcedure(message, params.procedure);

    return ok;
}