#include "Q4SMessage.h"
#include "Q4SMessageTools.h"
#include <sstream>


Q4SMessage::Q4SMessage( )
{
    clear();
}

Q4SMessage::~Q4SMessage( )
{
    done();
}
//initRequest1
bool Q4SMessage::initRequest( 
   Q4SMType q4SMType, 
   std::string host, 
   std::string port, 
   bool isSequenceNumber, unsigned long sequenceNumber, 
   bool isTimeStamp, uint64_t timeStamp, 
   bool isStage, unsigned long stage,
   bool isMeaurements,Q4SMeasurementValues *values)
 {
    done();

    bool ok = true;
    mQ4SMRequestOrResponse = Q4SMREQUESTORRESPOND_REQUEST;
    // FirstLine
    makeFirstLineRequest(q4SMType, host, port);
    // Headers
    
    makeHeaders(isSequenceNumber, sequenceNumber, isTimeStamp, timeStamp, isStage, stage, isMeaurements, values);
    //CRLF
    mMessage.append("\r\n");
    

    // Body
    makeBody(q4SMType);

    return ok;
}

//initRequest2


bool Q4SMessage::initResponse(Q4SResponseCode q4SResponseCode, std::string reasonPhrase)
{
    bool ok = true;

    mQ4SMRequestOrResponse = Q4SMREQUESTORRESPOND_RESPONSE;

    makeFirstLineResponse(q4SResponseCode, reasonPhrase); 

    // Headers
    //makeHeaders(isSequenceNumber, sequenceNumber, isTimeStamp, timeStamp, isStage, stage);
    mMessage.append("\r\n"); 
    //makeBody(q4SMType); 

    return ok; 
}

bool Q4SMessage::init200OKBeginResponse(Q4SSDPParams q4SSDPParams) 
{
    bool ok = true;
    //sprintf(mMesZZsage, "200 OK\n%s%lu/%lu\n",QOSLEVEL_PATTERN, params.qosLevelUp, params.qosLevelDown, ALERTINGMODE_PATTERN, params.q4SSDPAlertingMode);
    std::string SDP_message=Q4SSDP_create(q4SSDPParams); 
    std::string SDP_length_ms="Content-length:"; 
    unsigned long SDP_size=SDP_message.size(); 
    SDP_length_ms.append(std::to_string((unsigned long long int)SDP_size));
    SDP_length_ms.append("\r\n");

    mMessage="Q4S/1.0 200 OK\r\n";
    mMessage.append("Content-Type: application/sdp\r\n");
    mMessage.append(SDP_length_ms);
    mMessage.append("\r\n");
    mMessage.append(SDP_message);
    mMessage.append("\r\n");

    return ok;
}


bool Q4SMessage::initPing(std::string host, std::string port, unsigned long sequenceNumber, uint64_t timeStamp, bool results_flag, Q4SMeasurementValues *results)
{
    bool ok = true;
    if (results_flag)
    {
        ok &= initRequest(Q4SMTYPE_PING, host, port, true, sequenceNumber, true, timeStamp, false, 0, true, results);
    }
    else
    {
        ok &= initRequest(Q4SMTYPE_PING, host, port, true, sequenceNumber, true, timeStamp); 
    }
    //initRequest1
    return ok;
}

bool Q4SMessage::initBWidth(std::string host, std::string port, unsigned long sequenceNumber)
{
    bool ok = true;

    ok &= initRequest(Q4SMTYPE_BWIDTH, host, port, true, sequenceNumber);
    //initRequest3

    return ok;
}
std::string Q4SMessage::getMessage() const 
{
    return mMessage;
}

const char* Q4SMessage::getMessageCChar() const
{
    return mMessage.c_str();
}

void Q4SMessage::done( )
{
    clear();
}

// ---private----------------------------------------------------------------------------------------------------------------

void Q4SMessage::clear( )
{
    mMessage.clear();
}
void Q4SMessage::addVersion()
{
    mMessage.append("Q4S/1.0"); 
}


void Q4SMessage::makeFirstLineRequest(Q4SMType q4SMType, std::string host, std::string port)
{
    // Method
    makeFirstLineRequestMethod(q4SMType);
    mMessage.append(" ");

    // Request-URI
    makeFirstLineRequestURI(host, port);
    mMessage.append(" ");

    // Q4S-Version
    addVersion();
    mMessage.append("\r\n");
}

void Q4SMessage::makeFirstLineRequestMethod(Q4SMType q4SMType)
{
    std::string stringType = "NOT_DEFINED_TYPE";
    switch (q4SMType)
    {
        case Q4SMTYPE_BEGIN:
        {
            stringType = "BEGIN";
        }
        break;

        case Q4SMTYPE_READY:
        {
            stringType = "READY";
        }
        break;

        case Q4SMTYPE_PING:
        {
            stringType = "PING";
        }
        break;

        case Q4SMTYPE_BWIDTH:
        {
            stringType = "BWIDTH";
        }
        break;

        case Q4SMTYPE_CANCEL:
        {
            stringType = "CANCEL";
        }
        break;

        case Q4SMTYPE_Q4SALERT:
        {
            stringType = "Q4S-ALERT";
        }
        break;

        case Q4SMTYPE_INVALID:
        {
            stringType = "INVALID";
        }
        break;
    }

    mMessage.append(stringType);
}

void Q4SMessage::makeFirstLineRequestURI(std::string host, std::string port)
{
    std::string stringUri = "q4s:";
    stringUri.append("//");
    stringUri.append(host);
    stringUri.append(":");
    stringUri.append(port);

    mMessage.append(stringUri);
}

void Q4SMessage::makeHeaders(
    bool isSequenceNumber, unsigned long sequenceNumber, 
    bool isTimeStamp, uint64_t timeStamp, 
    bool isStage, unsigned long stage,
    bool isMeaurements,
    Q4SMeasurementValues *values)
{
    // TODO
     // Session-Id     
    //Sequence-Number
    if (isSequenceNumber)
    {
        mMessage.append("Sequence-Number:");
        mMessage.append(std::to_string((unsigned long long int)sequenceNumber));
        mMessage.append("\r\n");
    }

    //Timestamp
    if (isTimeStamp)
    {
        mMessage.append("Timestamp:");
        mMessage.append(std::to_string((uint64_t)timeStamp));
        mMessage.append("\r\n");
    }

    //Stage
    if (isStage)
    {
        mMessage.append("Stage:");
        mMessage.append(std::to_string((unsigned long long int)stage));
        mMessage.append("\r\n");
    }
    mMessage.append("Content-length:0\r\n");

    // Measurements
    if (isMeaurements)
    {     
        mMessage.append(Q4SMeasurementValues_create(*values));
        mMessage.append("\r\n");
        
    }
}
void Q4SMessage::makeBody(Q4SMType q4SMType)
{
     // TODO
    switch (q4SMType)
    {
        case Q4SMTYPE_BEGIN:
        {
        }
        break;

        case Q4SMTYPE_READY:
        {
        }
        break;

        case Q4SMTYPE_PING:
        {
        }
        break;

        case Q4SMTYPE_BWIDTH:
        {
           //Q4SMessageTools_fillBodyToASize(mMessage, 1024);
        }
        break;

        case Q4SMTYPE_CANCEL:
        {
        }
        break;

        case Q4SMTYPE_Q4SALERT:
        {
        }
        break;

        case Q4SMTYPE_INVALID:
        {
        }
        break;
    }
}
void Q4SMessage::makeFirstLineResponse(Q4SResponseCode q4SResponseCode, std::string reasonPhrase)
{
    // Q4S-Version
    addVersion();
    // SP
    mMessage.append(" ");
    // Status-Code
    makeFirstLineResponseStatusCode(q4SResponseCode);
    // SP
    mMessage.append(" ");
    // Reason-Phrase
    mMessage.append(reasonPhrase);

    // CRLF
    mMessage.append("\r\n");
}

void Q4SMessage::makeFirstLineResponseStatusCode(Q4SResponseCode q4SResponseCode)
{
switch (q4SResponseCode)
{
    case Q4SRESPONSECODE_200:
    {
        mMessage.append("200");
    }
    break;

    default:
    {
       // TODO
    }
    break;
}
}

std::string Q4SMeasurementValues_create(Q4SMeasurementValues results)
{
    std::string message;
    message.append(MEASUREMENTS_PATTERN);
    std::ostringstream streamLatency;
    streamLatency << results.latency;
    message.append(streamLatency.str());
    message.append(MEASUREMENTS_JITTER_PATTERN);
    std::ostringstream streamJitter;
    streamJitter << results.jitter;
    message.append(streamJitter.str());
    message.append(MEASUREMENTS_PACKETLOSS_PATTERN);
    std::ostringstream streamPacketLoss;
    streamPacketLoss << results.packetLoss;
    message.append(streamPacketLoss.str());
    message.append(MEASUREMENTS_BANDWIDTH_PATTERN);
    std::ostringstream streamBandwidth;
    streamBandwidth << results.bandwidth;
    message.append(streamBandwidth.str());
    //message.append("\n");
    return message;
 }
 
bool Q4SMeasurementValues_parse(std::string message, Q4SMeasurementValues &results)
{
    bool ok = true;

    std::string::size_type latencyPosition;
    std::string::size_type jitterPosition;
    std::string::size_type packetLossPosition;
    std::string::size_type bandwidthPosition;

    if (ok)
    {   
        std::string pattern = MEASUREMENTS_PATTERN;
        latencyPosition = message.find(pattern) + pattern.length();
        if (latencyPosition == std::string::npos) ok = false;
    }
    if (ok)
    {   
        std::string pattern = MEASUREMENTS_JITTER_PATTERN;
        jitterPosition = message.find(pattern, latencyPosition) + pattern.length();
        if (jitterPosition == std::string::npos) ok = false;
    }

    if (ok)
    {   
       std::string pattern = MEASUREMENTS_PACKETLOSS_PATTERN;
       packetLossPosition = message.find(pattern, jitterPosition) + pattern.length();
       if (packetLossPosition == std::string::npos) ok = false;
    }

    if (ok)
    {   
       std::string pattern = MEASUREMENTS_BANDWIDTH_PATTERN;
       bandwidthPosition    = message.find(pattern, packetLossPosition) + pattern.length();
       if (bandwidthPosition == std::string::npos) ok = false;
    }

    if (ok)
    {
        std::string latencyText      = message.substr(latencyPosition, (jitterPosition - 4 ) - latencyPosition);
        std::string jitterText       = message.substr(jitterPosition, (packetLossPosition - 5 ) - jitterPosition);
        std::string packetLossText   = message.substr(packetLossPosition, (bandwidthPosition - 5 ) - packetLossPosition);
        std::string bandwidthText    = message.substr(bandwidthPosition, message.length() - bandwidthPosition);
        results.latency      = std::stof(latencyText);
        results.jitter       = stof(jitterText);
        results.packetLoss   = stof(packetLossText);
        results.bandwidth    = stof(bandwidthText);

    }

    return ok;
}