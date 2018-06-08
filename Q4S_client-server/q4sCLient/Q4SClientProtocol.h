
#include "../q4sCommon/Q4SCommonProtocol.h"
#include "Q4SClientSocket.h"
#include "../q4sCommon/Q4SMessageManager.h"
#include <vector>
#include "../q4sCommon/Q4SStructs.h"
#include <signal.h>
#include <unistd.h>
class Q4SClientProtocol: Q4SCommonProtocol
{
public:

    // Constructor-Destructor
    Q4SClientProtocol ();
    ~Q4SClientProtocol ();

    // Init-Done
    bool    init(unsigned long times, unsigned long milisecondsBetweenTimes);    
    void    done();

    bool    ready(unsigned long stage,Q4SSDPParams &params);
    // Q4S Phases
    bool    handshake(Q4SSDPParams &params);
    void    alert();
    void    recovery();
    bool    negotiation(Q4SSDPParams params, Q4SMeasurementResult &results);
    void    continuity(Q4SSDPParams params);
    void    cancel();
private:


    void    clear();

    bool    openConnections();
    void    closeConnections();
    bool    tryOpenConnectionsTimes(unsigned long times, unsigned long milisecondsBetweenTimes);
    void    bwidth();

    bool    measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults,unsigned long pingsToSend);
    bool    measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend);
    bool    sendRegularPings(std::vector<uint64_t> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings);
    bool    measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults);

    bool            interchangeMeasurementProcedure(Q4SMeasurementValues &downMeasurements, Q4SMeasurementResult results);
    void*           manageBWReceivedData( );    
    static void*    manageBWReceivedDataFn( void* lpData);    
    void*           manageUdpReceivedData( );    
    static void*    manageUdpReceivedDataFn( void* lpData);
    void*           sendUDPBW();
    static void*    sendUDPBWFn(void* BWinfoFn );
    void*           sendUDPBW(unsigned long bandWidthDown);
    void*           manageTcpReceivedData( );
    static void*    manageTcpReceivedDataFn(void* lpData );
    unsigned long  bandWidthDown; 
    
	Q4SClientSocket             mClientSocket;    
    Q4SMessageManager           mReceivedMessagesTCP;    
    Q4SMessageManager           mReceivedMessagesUDP;
    Q4SMeasurementResult        mResults;
    pthread_t                   marrthrHandle[ 2 ]; 
    int                         qosLevel;
    int                         qosLevelMax; 
    uint64_t                    lastAlertTimeStamp;
    uint64_t                    recoveryTimeStamp;
    sem_t                       UDPSem; 
    pthread_t                   sendUDPBW_thread; 

};