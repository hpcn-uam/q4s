#ifndef _Q4SMESSAGE_H_
#define _Q4SMESSAGE_H_

#include <string>
#include <vector>
#include "Q4SSDP.h"
#include "Q4SMessageInfo.h"

#include "Q4SStructs.h"


#define MEASUREMENTS_PATTERN "Measurements: l="
#define MEASUREMENTS_JITTER_PATTERN ", j="
#define MEASUREMENTS_PACKETLOSS_PATTERN ", pl="
#define MEASUREMENTS_BANDWIDTH_PATTERN ", bw="

enum Q4SMRequestOrResponse
{ Q4SMREQUESTORRESPOND_INVALID
, Q4SMREQUESTORRESPOND_REQUEST
, Q4SMREQUESTORRESPOND_RESPONSE
};

enum Q4SMType
{ Q4SMTYPE_INVALID
, Q4SMTYPE_BEGIN
, Q4SMTYPE_READY
, Q4SMTYPE_PING
, Q4SMTYPE_BWIDTH
, Q4SMTYPE_CANCEL
, Q4SMTYPE_Q4SALERT
};

enum Q4SResponseCode
{ Q4SRESPONSECODE_INVALID
, Q4SRESPONSECODE_200
, Q4SRESPONSECODE_3XX
, Q4SRESPONSECODE_400
, Q4SRESPONSECODE_404
, Q4SRESPONSECODE_405
, Q4SRESPONSECODE_406
, Q4SRESPONSECODE_408
, Q4SRESPONSECODE_413
, Q4SRESPONSECODE_414
, Q4SRESPONSECODE_415
, Q4SRESPONSECODE_416
, Q4SRESPONSECODE_500
, Q4SRESPONSECODE_501
, Q4SRESPONSECODE_503
, Q4SRESPONSECODE_504
, Q4SRESPONSECODE_505
, Q4SRESPONSECODE_513
, Q4SRESPONSECODE_600
, Q4SRESPONSECODE_601
, Q4SRESPONSECODE_603
, Q4SRESPONSECODE_604
};

class Q4SMessage
{
    public:

    // Constructor-Destructor
    Q4SMessage( );
    ~Q4SMessage( );

    // Init-Done
    bool    initRequest(Q4SMType q4SMType, 
                        std::string host, 
                        std::string port,
                        bool isSequenceNumber=false,
                        unsigned long sequenceNumber=0,
                        bool isTimeStamp=false,
                        uint64_t timeStamp=0,
                        bool isStage=false,
                        unsigned long stage=0,
                        bool isMeaurements=false,
                        Q4SMeasurementValues *values=NULL,
                        bool incSDPparam=false, Q4SSDPParams *q4SSDPParams=NULL);

  

    bool    initResponse(Q4SResponseCode q4SResponseCode);
    bool    init200OKBeginResponse(Q4SSDPParams q4SSDPParams);
    bool    initPing(std::string host, std::string port, unsigned long sequenceNumber, uint64_t timeStamp, bool results_flag=false, Q4SMeasurementValues *results=NULL);
    bool    initBWidth(std::string host, std::string port, unsigned long sequenceNumber);
    void    done( );

    // Get Message
    std::string getMessage() const;
    const char* getMessageCChar() const;

private:

    void    clear( );

    void    addVersion();

    // Request
    void    makeFirstLineRequest(Q4SMType q4SMType, std::string host, std::string port);
    void    makeFirstLineRequestMethod(Q4SMType q4SMType);
    void    makeFirstLineRequestURI(std::string host, std::string port);
    void    makeHeaders(bool isSequenceNumber=false, 
                        unsigned long sequenceNumber=0, 
                        bool isTimeStamp=false, 
                        uint64_t timeStamp=0, 
                        bool isStage=false, 
                        unsigned long stage=0,
                        bool isMeaurements=false,
                        Q4SMeasurementValues *values=NULL,
                        bool incSDPparam=false, 
                        unsigned long SDP_size=0);
    void    makeBody(Q4SMType q4SMType);

    // Response 
    void    makeFirstLineResponse(Q4SResponseCode q4SResponseCode);
    void    makeFirstLineResponseStatusCode(Q4SResponseCode q4SResponseCode);


    std::string mMessage;
    Q4SMRequestOrResponse mQ4SMRequestOrResponse;
};

std::string Q4SMeasurementValues_create(Q4SMeasurementValues values);
bool Q4SMeasurementValues_parse(std::string message, Q4SMeasurementValues& values);


#endif // _Q4SMESSAGE_H_