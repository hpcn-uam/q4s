#ifndef _Q4SSERVERPROTOCOL_H_
#define _Q4SSERVERPROTOCOL_H_

#include "../q4sCommon/Q4SCommonProtocol.h"
#include "Q4SServerSocket.h"
#include "../q4sCommon/Q4SMessageManager.h"
#include <vector>
#include "../q4sCommon/Q4SStructs.h"
#include "../q4sCommon/Q4SSDP.h"


class Q4SServerProtocol:Q4SCommonProtocol
{
public:

    // Constructor-Destructor
    Q4SServerProtocol ();
    ~Q4SServerProtocol ();

    // Init-Done
    bool    init();
    void    done();

    bool    ready(Q4SSDPParams &params);

    void    end();
    void    alert(std::string alertMessage);
    void    recovery(std::string alertMessage);
    
    // Q4S Phases
    bool    handshake(Q4SSDPParams &params);
    bool    negotiation(Q4SSDPParams &params, Q4SMeasurementResult &results);
    void    continuity(Q4SSDPParams params);




private:

    void    clear();
    bool    openConnectionListening();
    void    closeConnectionListening();
    void    closeConnections();
    void    bwidth();
    void    cancel();
    bool    measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults, unsigned long pingsToSend);
    bool    measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults, unsigned long pingsToSend);
    bool    sendRegularPings(std::vector<uint64_t> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings);
    bool    measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult upResults);
    bool    interchangeMeasurementProcedure(Q4SMeasurementValues &upMeasurements, Q4SMeasurementResult results);
    Q4SServerSocket             mServerSocket;
    pthread_t                   marrthrListenHandle[ 2 ];
    pthread_t                   marrthrDataHandle[2];
    bool                        stop=false; 


    void*                       manageTcpConnection( );
    bool                        manageUdpConnection( );
    static void*                manageTcpConnectionsFn( void* lpData );
    static void*                manageUdpConnectionsFn( void* lpData );
    //static void*                checkConnectionsFn( void* lpData );

    struct ManageTcpConnectionsFnInfo
    {
        Q4SServerProtocol*      pThis;
        int                     connId;
    };    
    //void*                       checkConnections( int connId );
    void*                       manageTcpReceivedData( int connId );
    void*                       manageUdpReceivedData( );
    static void*                manageTcpReceivedDataFn( void* lpData );
    static void*                manageUdpReceivedDataFn( void* lpData );
    void*                       manageBWReceivedData( );    
    static void*                manageBWReceivedDataFn( void* lpData);
    void*                       sendUDPBW(unsigned long bandWidthUp);
    static void*                sendUDPBWFn(void* BWinfoFn );
    Q4SMessageManager           mReceivedMessagesTCP;    
    Q4SMessageManager           mReceivedMessagesUDP;
    unsigned long               bandWidthUp; 
    uint64_t                    lastAlertTimeStamp;
    uint64_t                    recoveryTimeStamp;
    //unsigned long               expiratedTimeStamp;
    pthread_mutex_t             mut_stop;
    int                         qosLevel;
    int                         qosLevelMax; 
    //bool                        flagEoS;    
    sem_t                       UDPSem; 
    pthread_t                   sendUDPBW_thread;  

};

#endif  // _Q4SSERVERPROTOCOL_H_