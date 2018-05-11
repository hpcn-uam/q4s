#include "Q4SClientProtocol.h"
#include "errno.h"
#include <stdio.h>
#include <sstream>
#include <sys/time.h>
//#include "ETime.h"
#include "../q4sCommon/Q4SMathUtils.h"
#include "Q4SClientConfigFile.h"
//#include "EKey.h"
#include "../q4sCommon/Q4SMessage.h"
#include "../q4sCommon/Q4SMessageTools.h"

Q4SClientProtocol::Q4SClientProtocol ()
{
    clear();
}

Q4SClientProtocol::~Q4SClientProtocol ()
{
    done();
}

bool Q4SClientProtocol::init(unsigned long times, unsigned long milisecondsBetweenTimes)
{
    // Prevention done call
    //done();

    bool ok = true;

    if (ok)
    {
        ok &= mReceivedMessages.init( );
    }

    if (ok)
    {
        ok &= tryOpenConnectionsTimes(times, milisecondsBetweenTimes);
    }

    return ok;
}

bool Q4SClientProtocol::tryOpenConnectionsTimes(unsigned long times, unsigned long milisecondsBetweenTimes)
{
    bool connected = false;

    unsigned long actualTime = 0;
    while ((actualTime<times) & !connected)
    {
        connected = openConnections();

        usleep(milisecondsBetweenTimes*1000);

        actualTime++;
    }

    return connected;
}

void Q4SClientProtocol::done()
{    
    mReceivedMessages.done( );
    closeConnections();
    
}

void Q4SClientProtocol::clear()
{
}
bool Q4SClientProtocol::openConnections()
{
    bool ok = true;
    int thread_error;

    //open connections
    if( ok )
    {
        ok &= mClientSocket.openConnection( SOCK_STREAM );
        ok &= mClientSocket.openConnection( SOCK_DGRAM );
    }

    // launch received data managing threads.
    thread_error = pthread_create( &marrthrHandle[0], NULL, manageUdpReceivedDataFn, ( void* ) this);
    thread_error = pthread_create( &marrthrHandle[1], NULL, manageTcpReceivedDataFn , ( void* ) this);
    return ok; 
}

void Q4SClientProtocol::closeConnections()
{
    bool ok = true;
    int join_error;
    if( ok )
    {
       
        pthread_cancel(marrthrHandle[0]);
        pthread_cancel(marrthrHandle[1]);
        ok &= mClientSocket.closeConnection( SOCK_STREAM );
        ok &= mClientSocket.closeConnection( SOCK_DGRAM );
        //WaitForMultipleObjects( 2, marrthrDataHandle, true, INFINITE );
        
    }
}
void Q4SClientProtocol::bwidth()
{
    printf("METHOD: bwidth TODO\n");
}
void Q4SClientProtocol::cancel()
{
    printf("METHOD: cancel TODO\n");
    mClientSocket.sendTcpData( "CANCEL\r\n" );
    //done(); 
}

bool Q4SClientProtocol::ready(unsigned long stage,Q4SSDPParams &params)
{
    printf("METHOD: ready\n");

    bool ok = true;
    
    if( ok )
    {
        Q4SMessage message;
        message.initRequest(Q4SMTYPE_READY, "myIP", q4SClientConfigFile.defaultTCPPort, false, 0, false, 0, true, stage);
        ok &= mClientSocket.sendTcpData( message.getMessageCChar() );
    }

    if ( ok )
    {
        std::string message;
        mReceivedMessages.readFirst( message );
        ok &= Q4SMessageTools_is200OKMessage(message);
        ok &= Q4SSDP_parse(message, params);
        qosLevel= params.qosLevelDown;
    }

    return ok;
}

bool Q4SClientProtocol::handshake(Q4SSDPParams &params)
{
    printf("----------Handshake Phase\n");
    printf("METHOD: begin\n");

    bool ok = true;
    
    if( ok )
    {
        Q4SMessage message;
        message.initRequest(Q4SMTYPE_BEGIN, "myIP", q4SClientConfigFile.defaultTCPPort);
        ok &= mClientSocket.sendTcpData( message.getMessageCChar() );
    }

    if ( ok ) 
    {
        std::string message;
        mReceivedMessages.readFirst( message );
        ok &= Q4SMessageTools_is200OKMessage(message);
        ok &= Q4SSDP_parse(message, params);
        qosLevel= params.qosLevelDown;
    }

    return ok;
}
bool Q4SClientProtocol::negotiation(Q4SSDPParams params, Q4SMeasurementResult &results)
{
    printf("----------Negotiation Phase\n");
    bool ok= true; 
    bool measureOk= false;     
    int QOS_negotiation;

    for( QOS_negotiation = 0; (measureOk  == false ) && ( QOS_negotiation!= 11 ); QOS_negotiation++ )
    {
        ok &= Q4SClientProtocol::ready(0, params);
        if( ok )
        {
            printf("MEASURING\n");
            Q4SMeasurementResult downResults;
            measureOk = Q4SClientProtocol::measureStage0(params, results, downResults, 20);
            if (measureOk)
            {
                ok &= Q4SClientProtocol::ready(1,params);
                if (ok)
                {
                    measureOk = Q4SClientProtocol::measureStage1(params, results, downResults);
                }
            }
        }

        if (measureOk)
        {
            mResults = results;
        }
    }    
     
    qosLevel= params.qosLevelUp;
    qosLevelMax= qosLevel; 
    if (!measureOk)
    {
        cancel(); 
        ok = false;  
    }
    return ok;
}

void Q4SClientProtocol::continuity(Q4SSDPParams params)
{
    printf("----------Continuity Phase\n");

    bool stop = false;
    bool measureOk = true;

    while ( !stop )
    {
        printf("MEASURING\n");

        Q4SMeasurementResult results;
        Q4SMeasurementResult downResults;

        measureOk = Q4SClientProtocol::measureContinuity(params, results, downResults, 20);

        // TODO notificar del error al Server
        if (!measureOk)
        {
            //Alert
            std::string alertMessage = "";

            if ( results.latencyAlert )
            {
                alertMessage += "Latency: " + std::to_string((long double)results.values.latency);
            }
            if ( results.jitterAlert )
            {
                alertMessage += " Jitter: " + std::to_string((long double)results.values.jitter);
            }

            if ( results.packetLossAlert)
            {
                alertMessage += " PacketLoss: " + std::to_string((long double)results.values.packetLoss);
            }

            printf(alertMessage.c_str());

            if (qosLevel==10)
            {
                printf("QOS: %d\n", qosLevel);
                stop= true; 
                cancel();
            }
            if (!stop)
            {
                alert(); 
            }
            showCheckMessage(results, downResults);
    
        }
        else
        {
            recovery(); 
        }

    }
}

void Q4SClientProtocol::recovery()
{
    if (qosLevel ==  qosLevelMax)
    {
       printf("No recovery send because QOS Level = %d\n", qosLevelMax);
    }
    else
    {
       if (lastAlertTimeStamp > recoveryTimeStamp)
       {
           recoveryTimeStamp = lastAlertTimeStamp;
       }

        struct timeval time_s;
        int time_error = gettimeofday(&time_s, NULL);
        unsigned long actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
            
       
       unsigned long timeForRecovery = actualTime - recoveryTimeStamp;
       if ( timeForRecovery > q4SClientConfigFile.recoveryPause)
       {
           qosLevel--;

           recoveryTimeStamp = actualTime;
           printf("METHOD: recovery\n");
           printf("QOS Level: %d\n", qosLevel);
       }
    }
}

void Q4SClientProtocol::alert()
{   
    struct timeval time_s;
    int time_error = gettimeofday(&time_s, NULL); 

    unsigned long actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
    unsigned long timeFromLastAlert = actualTime - lastAlertTimeStamp;
    if ( timeFromLastAlert > q4SClientConfigFile.alertPause)
    {
        qosLevel++;
        lastAlertTimeStamp = actualTime;


        printf("METHOD: alert\n");
        
    }
}

bool Q4SClientProtocol::measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend)
{
    bool ok = true;

    std::vector<unsigned long> arrSentPingTimestamps;
    Q4SMeasurementValues downMeasurements;

    if (ok)
    {
        // Send regular pings
        ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.negotiationTimeBetweenPingsUplink);
    }
    
    if(!ok)
    {
        printf( "ERROR:sendUdpData PING.\n" );
    }

    if (ok)
    {

        usleep(params.procedure.negotiationTimeBetweenPingsUplink *1000);

        // Calculate Latency
        calculateLatency(
            mReceivedMessages, 
            arrSentPingTimestamps, 
            results.values.latency, 
            pingsToSend, 
            q4SClientConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Latency Up:%.3f ms\n", results.values.latency );

        // Calculate Jitter
        float packetLoss = 0.f;
        calculateJitterStage0(
            mReceivedMessages, 
            results.values.jitter,
            params.procedure.negotiationTimeBetweenPingsUplink, 
            pingsToSend,
            q4SClientConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Jitter Up: %.3f ms\n", results.values.jitter );
    }

    if ( ok ) 
    {

        results.values.packetLoss= 0; 
        results.values.bandwidth= 0; 
        ok &= interchangeMeasurementProcedure(downMeasurements, results);
        printf( "MEASURING RESULT - Latency Down: %.3f ms\n", downMeasurements.latency );
        printf( "MEASURING RESULT - Jitter Down: %.3f ms\n", downMeasurements.jitter );
    }
    
    if ( ok ) 
    {
        downResults.values = downMeasurements;
        ok &= checkStage0(params.latency, params.jitterUp, params.latency, params.jitterDown, results, downResults);
    }

    if (!ok)
    {
        showCheckMessage(results, downResults);
    }

    return ok;
}

bool Q4SClientProtocol::interchangeMeasurementProcedure(Q4SMeasurementValues &downMeasurements, Q4SMeasurementResult results)
{
    bool ok = true;

    if ( ok ) 
    {
        // Send Info Ping with sequenceNumber 0
        Q4SMessage infoPingMessage;
        ok &= infoPingMessage.initPing("myIp", q4SClientConfigFile.defaultUDPPort, 0, 0, results.values);
        ok &= mClientSocket.sendTcpData( infoPingMessage.getMessageCChar() );
    }
    
    if (ok)
    {
        // Wait to recive the measurements Ping
        Q4SMessageInfo  messageInfo;
        ok &= mReceivedMessages.readPingMessage( 0, messageInfo, true );
        if (ok)
        {
            ok &= Q4SMeasurementValues_parse(messageInfo.message, downMeasurements);
            if (!ok)
            {
                printf( "ERROR:Interchange Read measurements fail\n");
            }
        }
        else
        {
            printf( "ERROR:Interchange Read PING fail\n");
            printf( "Messages:\n");
            std::string toPrint;
            while (mReceivedMessages.readFirst(toPrint))
            {
                printf(toPrint.c_str());
            }
        }
    }

    return ok;
}

bool Q4SClientProtocol::measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend)
{
    bool ok = true;

    std::vector<unsigned long> arrSentPingTimestamps;
    Q4SMeasurementValues downMeasurements;

    if (ok)
    {
        // Send regular pings
        ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.continuityTimeBetweenPingsUplink);
    }
    
    if(!ok)
    {
        printf( "ERROR:sendUdpData PING.\n" );
    }

    if (ok)
    {
        usleep(params.procedure.negotiationTimeBetweenPingsUplink *1000);

        // Calculate Latency
        calculateLatency(
            mReceivedMessages, 
            arrSentPingTimestamps, 
            results.values.latency, 
            pingsToSend, 
            q4SClientConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Latency Up: %.3f ms\n", results.values.latency );

        // Calculate Jitter
        calculateJitterAndPacketLossContinuity(
            mReceivedMessages, 
            results.values.jitter, 
            params.procedure.continuityTimeBetweenPingsUplink, 
            pingsToSend,
            results.values.packetLoss,
            q4SClientConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Jitter Up: %.3f ms\n", results.values.jitter );
        printf( "MEASURING RESULT - PacketLoss Up: %.3f %\n", results.values.packetLoss );
    }

    if (ok)
    {
        results.values.bandwidth= 0; 
        ok &= interchangeMeasurementProcedure(downMeasurements, results);
        printf( "MEASURING RESULT - Latency Down: %.3f ms\n", downMeasurements.latency );
        printf( "MEASURING RESULT - Jitter Down: %.3f ms\n", downMeasurements.jitter );
        printf( "MEASURING RESULT - PacketLoss Down: %.3f %\n", downMeasurements.packetLoss );
    }

    if (ok)
    {
        // Check 
        downResults.values = downMeasurements;

        ok &= Q4SCommonProtocol::checkContinuity(
            params.latency, params.jitterUp, params.packetLossUp, 
            params.latency, params.jitterDown, params.packetLossDown,
            results,
            downResults);
    }
    
    

    return ok;
}

bool Q4SClientProtocol::sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings)
{
    bool ok = true;

    Q4SMessage message;
    unsigned long timeStamp = 0;
    int pingNumber = 0;
    int pingNumberToSend = pingsToSend;
    int time_error ;
    struct timeval time_s;
    for ( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
    {
        // Store the timestamp
        time_error = gettimeofday(&time_s, NULL); 
        timeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
        //timeStamp = ETime_getTime( );
        
        arrSentPingTimestamps.push_back( timeStamp );

        // Prepare message and send
        message.initPing("myIp", q4SClientConfigFile.defaultUDPPort, pingNumber, timeStamp);
        ok &= mClientSocket.sendUdpData( message.getMessageCChar() );

        // Wait the established time between pings

        
        usleep(timeBetweenPings*1000 );
    }
   

    return ok;
}

bool Q4SClientProtocol::measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults)
{
    bool ok = true;

    Q4SMeasurementValues downMeasurements;

    printf( "Starting:measureStage1.\n" );

    Q4SMessage message;
    struct timeval time_s;
    int time_error = gettimeofday(&time_s, NULL); 
    struct timeval time_t_aux;
    time_error = gettimeofday(&time_t_aux, NULL); 
    unsigned long TimeStamp2 =  time_t_aux.tv_sec*1000 + time_t_aux.tv_usec/1000;
    unsigned long sequenceNumber = 0;
    unsigned long TimeStamp3;
    unsigned long initialTimeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;   

    while(ok && (TimeStamp2 < initialTimeStamp + params.procedure.bandwidthTime))
    {
        time_error = gettimeofday(&time_t_aux, NULL); 
        TimeStamp3 =  time_t_aux.tv_sec*1000 + time_t_aux.tv_usec/1000;

        ok &= message.initRequest(Q4SMTYPE_BWIDTH, "myIp", q4SClientConfigFile.defaultUDPPort, true, sequenceNumber, true, TimeStamp3);
        ok &= mClientSocket.sendUdpData( message.getMessageCChar() );
        time_error = gettimeofday(&time_t_aux,NULL); 
        TimeStamp2 =  time_t_aux.tv_sec*1000 + time_t_aux.tv_usec/1000;
        sequenceNumber++;
    }

    if (ok)
    {
        calculateBandwidthStage1(sequenceNumber, params.procedure.bandwidthTime, results.values.bandwidth);
        printf( "MEASURING RESULT - Bandwidth Up: %0.2f Kb/s\n", results.values.bandwidth );
    }

    if (ok)
    {
        // Calculate PacketLoss
        bool okCalculated = calculatePacketLossStage1(mReceivedMessages, results.values.packetLoss);
        if (!okCalculated)
        {
            printf( "PacketLoss Calculation Error");
        }

        printf( "MEASURING RESULT - PacketLoss Up: %.3f %\n", results.values.packetLoss );
    }

    if(!ok)
    {
        printf( "ERROR:sendUdpData BWidth.\n" );
    }

    if (ok)
    {
        ok &= interchangeMeasurementProcedure(downMeasurements, results);
        printf( "MEASURING RESULT - Bandwidth Down: %.3f Kb/s\n", downMeasurements.bandwidth);
        printf( "MEASURING RESULT - PacketLoss Down: %.3f %\n", downMeasurements.packetLoss );
    }

    if (ok)
    {
        // Check
        downResults.values = downMeasurements;
        ok &= checkStage1(params.bandWidthUp, params.packetLossUp, params.bandWidthDown, params.packetLossDown, results, downResults);
    }

    if (!ok)
    {
        showCheckMessage(results, downResults);
    }

    return ok;
}

void* Q4SClientProtocol::manageTcpReceivedDataFn( void* lpData )
{
	Q4SClientProtocol* q4sCP = ( Q4SClientProtocol* )lpData;
    bool ret = q4sCP->manageTcpReceivedData( );
}

void* Q4SClientProtocol::manageUdpReceivedDataFn( void* lpData )
{
	Q4SClientProtocol* q4sCP = ( Q4SClientProtocol* )lpData;
    bool ret = q4sCP->manageUdpReceivedData( );
    
}

void* Q4SClientProtocol::manageUdpReceivedData( )
{
    bool            ok = true;
    char            udpBuffer[ 65536 ];
    int             time_error;
    int             pingNumber; 
    unsigned long   actualTimeStamp;
    unsigned long   receivedTimeStamp;
    struct          timeval time_s;

    while ( ok )
    {
        ok &= mClientSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ) );

        if( ok )
        {
            timeval time_s;
            time_error = gettimeofday(&time_s, NULL); 
            actualTimeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;

            std::string message = udpBuffer;

            pingNumber = 0;

            // Comprobar que es un ping
            if ( Q4SMessageTools_isPingMessage(udpBuffer, &pingNumber, &receivedTimeStamp) )
            {

                if (q4SClientConfigFile.showReceivedPingInfo)
                {
                    printf( "Received Ping, number:%d, timeStamp: %d\n", pingNumber, receivedTimeStamp);
                }

                // mandar respuesta del ping
                char reasonPhrase[ 256 ];
                if (q4SClientConfigFile.showReceivedPingInfo)
                {
                    printf( "Ping responsed %d\n", pingNumber);
                }
                Q4SMessage message200;
                sprintf( reasonPhrase, "OK %d", pingNumber );
                ok &= message200.initResponse(Q4SRESPONSECODE_200, reasonPhrase);
                ok &= mClientSocket.sendUdpData(message200.getMessageCChar());
                // encolar el ping y el timestamp para el calculo del jitter
                mReceivedMessages.addMessage(message, actualTimeStamp);
            }
            else
            {
                // encolar el 200 ok y el timestamp actual para el calculo de la latencia
                mReceivedMessages.addMessage(message, actualTimeStamp);
            }



            

            if (q4SClientConfigFile.showReceivedPingInfo)
            {
                printf( "Received Udp: <%s>\n", udpBuffer );
            }
        }

/*// Key management
        if (EKey_getKeyState(EK_C))
        {
            printf( "CANCEL key pressed\n");
            cancel();
            ok = false;
        }
    */
    } 
}

void* Q4SClientProtocol::manageTcpReceivedData( )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    
    while (ok) 
    {
        ok &= mClientSocket.receiveTcpData( buffer, sizeof( buffer ) );
        if( ok )
        {
            std::string message = buffer;
            mReceivedMessages.addMessage ( message );
        }
    }

}
