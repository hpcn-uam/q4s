
#include "../q4sCommon/Q4SCommonProtocol.h"
#include "Q4SClientSocket.h"
#include "../q4sCommon/Q4SMessageManager.h"
#include <vector>
#include "../q4sCommon/Q4SStructs.h"
#include <signal.h>
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
private:


    void    clear();

    bool    openConnections();
    void    closeConnections();
    bool    tryOpenConnectionsTimes(unsigned long times, unsigned long milisecondsBetweenTimes);
    void    bwidth();
    void    cancel();
    bool    measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults,unsigned long pingsToSend);
    bool   measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend);
    bool    sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings);
    bool    measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults);

    bool            interchangeMeasurementProcedure(Q4SMeasurementValues &downMeasurements, Q4SMeasurementResult results);
    void            sighand(int signo);
    void*           manageUdpReceivedData( );
    static void*    manageUdpReceivedDataFn( void* lpData);
    void*           manageTcpReceivedData( );
    static void*    manageTcpReceivedDataFn(void* lpData );

	Q4SClientSocket             mClientSocket;    
    Q4SMessageManager           mReceivedMessages;
    Q4SMeasurementResult        mResults;
    pthread_t                   marrthrHandle[ 2 ]; 
    struct sigaction            actions;
    int                         qosLevel;
    int                         qosLevelMax; 
    unsigned long               lastAlertTimeStamp;
    unsigned long               recoveryTimeStamp;

};