#include "Q4SServerProtocol.h"

#include <stdio.h>
#include <vector>
#include <sstream>
#include <math.h>
//#include "ETime.h"
#include "../q4sCommon/Q4SMathUtils.h"
#include "Q4SServerConfigFile.h"
#include <sys/time.h>

//#include "EKey.h"
#include "../q4sCommon/Q4SMessage.h"
#include "../q4sCommon/Q4SMessageTools.h"

#define     DEFAULT_CONN_ID     1

Q4SServerProtocol::Q4SServerProtocol ()
{
    clear();
}

Q4SServerProtocol::~Q4SServerProtocol ()
{
    done();
}

bool Q4SServerProtocol::init()
{
    // Prevention done call
    //done();

    bool ok = true;
    bool okReceivedMessages = true;
    //flagEoS= false; 
    #if SAVE_INFO
        remove( "../../test/measured/measure_server.txt" ); 
    #endif
    if (ok)
    {
        okReceivedMessages = mReceivedMessagesUDP.init( ); 
        okReceivedMessages = mReceivedMessagesTCP.init( ); 

    }

    if (ok)
    {
        ok &= openConnectionListening(); 
    }

    if (ok)
    {
        ok &= mServerSocket.startAlertSender();
    }
    if (ok)
    {
        sem_init(&UDPSem, 0, 0);

    }

    return ok;
}

void Q4SServerProtocol::done()
{

    closeConnections();
    //mReceivedMessagesUDP.done( );
    //mReceivedMessagesTCP.done( );

    //pthread_mutex_destroy(&mut_stop); 



    
}

void Q4SServerProtocol::clear()
{
    //lastAlertTimeStamp = ULONG_MAX;
}

bool Q4SServerProtocol::openConnectionListening()
{
    bool ok = true;
    int thread_error;    
    //thread_error= pthread_mutex_init(&mut_stop, NULL);

    thread_error = pthread_create( &marrthrListenHandle[0], NULL, manageTcpConnectionsFn, ( void* ) this);
    if (thread_error< 0)
        {
            printf("ERRor marrthrListenHandle[0]\n");
        }
        
    thread_error = pthread_create( &marrthrListenHandle[1], NULL,manageUdpReceivedDataFn , ( void* ) this);
    
    if (thread_error< 0)
        {
            printf("ERRor marrthrListenHandle[1]\n");
        }

    
  
    return ok;
}


void Q4SServerProtocol::closeConnectionListening()
{
    bool ok = true;

    if( ok )
    {
        //mServerSocket.stopWaiting( );
        //pthread_join( marrthrListenHandle[0], NULL);
        //pthread_join( marrthrListenHandle[1], NULL);
    }
     
        /*
        pthread_mutex_destroy(&mut_flag);
        pthread_mutex_destroy(&mut_Timestamp);
        */

        //pthread_join( marrthrListenHandle[2], NULL);
    

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}

void Q4SServerProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        ok &= mServerSocket.closeConnection( SOCK_STREAM );
        //ok &= mServerSocket.closeConnection( SOCK_DGRAM );
        //WaitForMultipleObjects( 2, marrthrDataHandle, true, INFINITE );
        pthread_join( marrthrDataHandle[0], NULL);
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}
bool Q4SServerProtocol::handshake(Q4SSDPParams &params)
{
    //printf("----------Handshake Phase\n");
    //printf("WAITING FOR BEGIN\n");
    std::string message;
            //printf("MEASURING\n");
    bool okCancel= true; 
    bool ok = false;                 
    
    while(mReceivedMessagesUDP.size()>0)
    {
        mReceivedMessagesUDP.eraseMessages();
    }    
    while(mReceivedMessagesTCP.size()>0)
    {
        mReceivedMessagesTCP.eraseMessages();
    }
    
    //printf("handshake 0\n");
    while(!ok && okCancel) 
    {
        
        //printf("Handshake\n");
        // Wait for a message    
            okCancel &= !mReceivedMessagesTCP.readCancelMessage();
            //printf("okCancel= %d", okCancel);
            ok=mReceivedMessagesTCP.readFirst( message );
        
        if (ok)
        {
            std::string pattern;
            pattern.assign( "BEGIN" );
            if (message.substr( 0, pattern.size() ).compare( pattern ) != 0)
            {
                ok = false;
            }
        }
    }
    if(ok)
    {
        Q4SMessage message200;
        params.qosLevelUp = 0;
        params.qosLevelDown = 0;
        if (q4SServerConfigFile.isReactive)
        {
           params.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_REACTIVE;
        }
        else
        {
           params.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
        }
        params.alertPause = q4SServerConfigFile.alertPause;
        params.recoveryPause = q4SServerConfigFile.recoveryPause;
        params.latency = q4SServerConfigFile.latency;
        params.jitterUp = q4SServerConfigFile.jitterUp;
        params.jitterDown = q4SServerConfigFile.jitterDown;
        params.bandWidthUp = q4SServerConfigFile.bandwidthUp[0];
        params.bandWidthDown = q4SServerConfigFile.bandwidthDown[0];
        params.packetLossUp = q4SServerConfigFile.packetLossUp;
        params.packetLossDown = q4SServerConfigFile.packetLossDown;
        params.procedure.negotiationTimeBetweenPingsUplink = q4SServerConfigFile.timeBetweenPingsNegotiationUp;
        params.procedure.negotiationTimeBetweenPingsDownlink = q4SServerConfigFile.timeBetweenPingsNegotiationDown;
        params.procedure.continuityTimeBetweenPingsUplink = q4SServerConfigFile.timeBetweenPingsContinuityUp;
        params.procedure.continuityTimeBetweenPingsDownlink = q4SServerConfigFile.timeBetweenPingsContinuityDown;
        params.procedure.bandwidthTime = q4SServerConfigFile.bandwidthTime;
        params.procedure.windowSizeLatencyCalcUplink= q4SServerConfigFile.numberOfPingsUp;
        params.procedure.windowSizeLatencyCalcDownlink= q4SServerConfigFile.numberOfPingsDown;
        params.procedure.windowSizePacketLossCalcUplink= q4SServerConfigFile.numberOfBwidthsUp;
        params.procedure.windowSizePacketLossCalcDownlink= q4SServerConfigFile.numberOfBwidthsDown;
        ok &= message200.init200OKBeginResponse(params);
        ok &= mServerSocket.sendTcpData( DEFAULT_CONN_ID, message200.getMessageCChar());
        
    }
    
    return ok;
}

 bool Q4SServerProtocol::negotiation(Q4SSDPParams &params, Q4SMeasurementResult &results) 
 {
    //printf("----------Negotiation Phase\n");
    bool ok = true;
    int QOS_negotiation;
    bool measureOk= false; 
    char data2save[200]={}; 
    /*
    pthread_mutex_lock (&mut_flag);
    bool flagEoS_negotiation=flagEoS;
    pthread_mutex_unlock (&mut_flag);
    */
    Q4SMeasurementResult upResults;
    for( QOS_negotiation = 0; (measureOk  == false ) && ( QOS_negotiation!= 11 ) && (ok==true) ; QOS_negotiation++ )
    {
        #if SHOW_INFO
            printf("QOS level: %d\n", QOS_negotiation);
        #endif 
        if (ok)
        {        
               
            ok &= Q4SServerProtocol::ready(params);

            //printf("MEASURING\n");
            params.bandWidthUp = q4SServerConfigFile.bandwidthUp[QOS_negotiation];
            params.bandWidthDown = q4SServerConfigFile.bandwidthDown[QOS_negotiation];
            params.qosLevelUp = QOS_negotiation;
            params.qosLevelDown = QOS_negotiation;
            measureOk = Q4SServerProtocol::measureStage0(params, results, upResults, 20);    
            ok &= !mReceivedMessagesTCP.readCancelMessage();

            if (measureOk && ok)
            {
               ok &= Q4SServerProtocol::ready(params);
               if (ok)
               {
                    measureOk = Q4SServerProtocol::measureStage1(params, results, upResults);
               }
            }
            if(!measureOk && ok)
            {
                std::string alertMessage;
                alertMessage.append("Latency: " + std::to_string((long double)results.values.latency));
                alertMessage.append(" Jitter: " + std::to_string((long double)results.values.jitter));
                alertMessage.append(" PacketLoss: " + std::to_string((long double)results.values.packetLoss));
                alertMessage.append(" BandWidth: " + std::to_string((long double)results.values.bandwidth));
                //Alert
                Q4SServerProtocol::alert(alertMessage);
            }           
        }
        else 
        {
            break; 
        }

        qosLevel= QOS_negotiation; 
        #if SAVE_INFO     
            struct timeval time_s;   
            std::ofstream pFile("../../test/measured/measure_server.txt", ios::out | ios::app);;         
            int time_error = gettimeofday(&time_s, NULL); 
            uint64_t actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
            sprintf(data2save, "0\t%"PRIu64"\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%d\t%d\n",actualTime,results.values.latency,upResults.values.latency, results.values.jitter, upResults.values.jitter,results.values.bandwidth, upResults.values.bandwidth,results.values.packetLoss, upResults.values.packetLoss,qosLevel, measureOk); 
            pFile<<data2save;
            //fclose(pFile); 
        #endif
    }
        qosLevelMax= qosLevel; 
          
        ok&= !mReceivedMessagesTCP.readCancelMessage();
        ok &= measureOk; 
        return ok;
 }
void Q4SServerProtocol::continuity(Q4SSDPParams params)
{
    //printf("----------Continuity Phase\n");
    bool measureOk = true;
    bool stop1= false; 
    /*
    pthread_mutex_lock (&mut_flag);
    bool flagEoS_continuity=flagEoS;
    pthread_mutex_unlock (&mut_flag);
    */
    std::string alertMessage;
    char data2save[200]={}; 
    Q4SMeasurementResult upResults;
    Q4SMeasurementResult results;

    while ( !stop1)
    {
        /*
        pthread_mutex_lock (&mut_flag);
        flagEoS_continuity=flagEoS;
        pthread_mutex_unlock (&mut_flag);
        */
        //printf("MEASURING\n");
        stop1 = mReceivedMessagesTCP.readCancelMessage();

        measureOk = Q4SServerProtocol::measureContinuity(params, results, upResults, 20);
        if (!measureOk)
        {
            //Alert
            alertMessage = generateNotificationAlertMessage(params, upResults, results);
            alert(alertMessage);
        }
        else
        {
           // Recovery
           alertMessage = generateNotificationAlertMessage(params, upResults, results);
           recovery(alertMessage);
        }
     #if SAVE_INFO
        struct timeval time_s;
        std::ofstream pFile("../../test/measured/measure_server.txt", ios::out | ios::app);; 
        //pFile = fopen ("measure_client.txt","w");    
        int time_error = gettimeofday(&time_s, NULL); 
        uint64_t actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
        sprintf(data2save, "1\t%"PRIu64"\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%d\t%d\n",actualTime,results.values.latency,upResults.values.latency, results.values.jitter, upResults.values.jitter,results.values.bandwidth, upResults.values.bandwidth,results.values.packetLoss, upResults.values.packetLoss,qosLevel, measureOk); 
        pFile<<data2save;
        //fclose(pFile); 
    #endif
    }
}

bool Q4SServerProtocol::ready(Q4SSDPParams &params)
{
    std::string message;
    bool ok = false;
    bool okCancel = true; 
    while (!ok && okCancel)
    {
        okCancel &= !mReceivedMessagesTCP.readCancelMessage();
        if (okCancel)
        {
            ok = mReceivedMessagesTCP.readFirst( message );
        }   
        //printf("No hay mensaje\n");


        if (ok)
        {
            std::string patternReady;
            patternReady.assign( "READY" );
            if ( message.substr( 0, patternReady.size( ) ).compare( patternReady ) != 0)
            {
                ok = false;
            }
            else
            {
                std::string patternStage;
                patternStage.assign("Stage:");
                if ( message.find( patternStage) == std::string::npos)
                {
                   ok = false;
                }
            }
        }
    }
    while(mReceivedMessagesUDP.size()>0)
    {
        //printf("BORRANDO MENSAJES\n");
        mReceivedMessagesUDP.eraseMessages();
    }
    if( ok )
    {
        Q4SMessage message200;
        ok &= message200.init200OKBeginResponse(params);
        ok &= mServerSocket.sendTcpData( DEFAULT_CONN_ID, message200.getMessageCChar());
    }
    return ok;
}

void Q4SServerProtocol::alert(std::string alertMessage)
{   
    struct timeval time_s;
    int time_error = gettimeofday(&time_s, NULL); 
    uint64_t actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
    uint64_t timeFromLastAlert = actualTime - lastAlertTimeStamp;

    if ( timeFromLastAlert > q4SServerConfigFile.alertPause)
    {
        qosLevel++;
        lastAlertTimeStamp = actualTime;
        std::string message = "ALERT "+alertMessage;
        #if SHOW_INFO
        printf("ENVIO ALERT\n");
        #endif
        mServerSocket.sendAlertData(message.c_str());
    }
}

void Q4SServerProtocol::end()
{
    closeConnections();
}

void Q4SServerProtocol::recovery(std::string recoveryMessage)
{
    if (qosLevel ==  qosLevelMax)
    {
        #if SHOW_INFO
            printf("No recovery send because QOS Level = %d\n", qosLevelMax);
        #endif
    }
    else
    {
        if (lastAlertTimeStamp > recoveryTimeStamp)
        {
           recoveryTimeStamp = lastAlertTimeStamp;
        }

        struct timeval time_s;
        int time_error = gettimeofday(&time_s, NULL);
        uint64_t actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;     
        uint64_t timeForRecovery = actualTime - recoveryTimeStamp;

        if ( timeForRecovery > q4SServerConfigFile.recoveryPause)
        {
            qosLevel--;
            recoveryTimeStamp = actualTime;
            std::string message = "RECOVERY " + recoveryMessage;
            mServerSocket.sendAlertData(message.c_str());
            #if SHOW_INFO
            printf("METHOD: recovery\n");
            printf("QOS Level: %d\n", qosLevel);
            #endif
        }
    }
}


bool Q4SServerProtocol::sendRegularPings(std::vector<uint64_t> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings)
{
    bool ok = true;
    Q4SMessage message;
    uint64_t timeStamp = 0;
    int pingNumber = 0;
    int pingNumberToSend = pingsToSend;
    int time_error;

    for( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
    {
        // Store the timestamp
        struct timeval time_s;
        time_error = gettimeofday(&time_s, NULL); 
        timeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;       
        arrSentPingTimestamps.push_back( timeStamp );
        // Prepare message and send
        message.initPing("myIp", q4SServerConfigFile.defaultUDPPort, pingNumber, timeStamp);
        ok &= mServerSocket.sendUdpData( DEFAULT_CONN_ID, message.getMessageCChar() );      
        // Wait the established time between pings
        usleep(timeBetweenPings*1000 );
    }
    return ok;
}

bool Q4SServerProtocol::measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults, unsigned long pingsToSend)
{
    bool ok = true;
    std::vector<uint64_t> arrSentPingTimestamps;
    Q4SMeasurementValues upMeasurements;
    ok &= !mReceivedMessagesTCP.readCancelMessage();
    int firstPing; 
    if ( ok ) 
    {
        // Wait to receive the first Ping
        Q4SMessageInfo  messageInfo;
        ok= false; 
        for (firstPing= 0; ok==false && firstPing<=pingsToSend; firstPing++)
        {
        //printf("firstPing:%d\n", firstPing );
            ok = mReceivedMessagesUDP.readPingMessage( firstPing, messageInfo, false );
        }
    }

    if(!ok)
    {
        printf( "ERROR:PING 0 is not the first message.\n" );
    }

    if( ok )
    {
        // Send regular pings
        ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.negotiationTimeBetweenPingsDownlink);
    }

    if(!ok)
    {
        printf( "ERROR:sendUdpData PING.\n" );
    }

    if (ok)
    {
        usleep(params.procedure.negotiationTimeBetweenPingsUplink*1000);
        // Calculate Latency
        calculateLatency(
            mReceivedMessagesUDP, 
            arrSentPingTimestamps, 
            results.values.latency, 
            pingsToSend, 
            q4SServerConfigFile.showMeasureInfo);
        // Calculate Jitter
        calculateJitterStage0(
            mReceivedMessagesUDP, 
            results.values.jitter,
            params.procedure.negotiationTimeBetweenPingsDownlink, 
            pingsToSend,
            q4SServerConfigFile.showMeasureInfo);             
    }
    ok &= !mReceivedMessagesTCP.readCancelMessage();

    if ( ok ) 
    {
        results.values.packetLoss= 0;  
        results.values.bandwidth= 0; 
        ok &= interchangeMeasurementProcedure(upMeasurements, results);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Latency Down: %.3f ms\n", results.values.latency );
            printf( "MEASURING RESULT - Jitter Down: %.3f ms\n", results.values.jitter );
            printf( "MEASURING RESULT - Latency Up: %.3f ms\n", upMeasurements.latency );
            printf( "MEASURING RESULT - Jitter Up: %.3f ms\n", upMeasurements.jitter );
        #endif
    }
    if ( ok )
    {
        upResults.values = upMeasurements;
        ok &= checkStage0(params.latency, params.jitterUp, params.latency, params.jitterDown, upResults, results);
    }

    if (!ok)
    {
        showCheckMessage(upResults, results);
    }
    return ok;
}

bool Q4SServerProtocol::interchangeMeasurementProcedure(Q4SMeasurementValues &upMeasurements, Q4SMeasurementResult results)
{
    bool ok = false;
    Q4SMessageInfo  messageInfo;

    while(!ok)
    {
        ok = mReceivedMessagesTCP.readPingMessage( 0, messageInfo, true );
        if (ok)
        {
            ok &= Q4SMeasurementValues_parse(messageInfo.message, upMeasurements);
            if (!ok)
            {
                printf( "ERROR:Interchange Read measurements fail\n");
            }
        }      
    }
    
   

    if ( ok )
    {
        // Send Info Ping with sequenceNumber 0
        Q4SMessage infoPingMessage;
        ok &= infoPingMessage.initPing("myIp", q4SServerConfigFile.defaultUDPPort, 0, 0, results.values);
        ok &= mServerSocket.sendTcpData(DEFAULT_CONN_ID, infoPingMessage.getMessageCChar());
    }

    return ok;
}
bool Q4SServerProtocol::measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults, unsigned long pingsToSend)
{
    bool ok = true;
    std::vector<uint64_t> arrSentPingTimestamps;
    Q4SMeasurementValues upMeasurements;
    ok &= !mReceivedMessagesTCP.readCancelMessage();

    if ( ok ) 
    {
        // Wait to recive the first Ping
        Q4SMessageInfo  messageInfo;
        ok &= mReceivedMessagesUDP.readPingMessage( 0, messageInfo, false );
    }

    if(!ok)
    {
        printf( "ERROR:PING 0 is not the first message.\n" );
    }

    if( ok )
    {
        // Send regular pings
        ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.continuityTimeBetweenPingsDownlink);

    }
    if(!ok)
    {

        printf( "ERROR:sendUdpData PING.\n" );
    }

    if (ok)
    {
        // Wait the established time to start calculation
        usleep(params.procedure.negotiationTimeBetweenPingsUplink *1000);
        // Calculate Latency
        calculateLatency(mReceivedMessagesUDP, arrSentPingTimestamps, results.values.latency, pingsToSend, q4SServerConfigFile.showMeasureInfo);
        // Calculate Jitter
        calculateJitterAndPacketLossContinuity(mReceivedMessagesUDP, results.values.jitter, params.procedure.continuityTimeBetweenPingsDownlink, pingsToSend,results.values.packetLoss, q4SServerConfigFile.showMeasureInfo);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Latency Down: %.3f ms\n", results.values.latency );
            printf( "MEASURING RESULT - Jitter Down: %.3f ms\n", results.values.jitter );
            printf( "MEASURING RESULT - PacketLoss Down: %.3f %\n", results.values.packetLoss );
        #endif
    }
        // Check latency and jitter limits
    ok &= !mReceivedMessagesTCP.readCancelMessage();
    //printf("%d\n", mReceivedMessagesTCP.readCancelMessage(), ok);
    if (ok)
    {
//printf("DENTRO\n");
        ok &= interchangeMeasurementProcedure(upMeasurements, results);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Latency Up: %.3f ms\n", upMeasurements.latency );
            printf( "MEASURING RESULT - Jitter Up: %.3f ms\n", upMeasurements.jitter );
            printf( "MEASURING RESULT - PacketLoss Up: %.3f %\n", upMeasurements.packetLoss );
        #endif
    }
    if (ok)
    {
        // Check
        upResults.values = upMeasurements;
        ok &= Q4SCommonProtocol::checkContinuity(
            params.latency, params.jitterUp, params.packetLossUp,
            params.latency, params.jitterDown, params.packetLossDown,
            upResults,
            results);
    }
    if (!ok)
    {
        showCheckMessage(upResults, results);  
    } 
    return ok;
}

bool Q4SServerProtocol::measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult upResults)
{
    bool ok = true;
    Q4SMeasurementValues upMeasurements;
    Q4SMessage message;    
    bandWidthUp= params.bandWidthUp;
    pthread_create(&sendUDPBW_thread, NULL, sendUDPBWFn, ( void* ) this);
    int interval= 0; 
    ok &= !mReceivedMessagesTCP.readCancelMessage();

    while(ok && interval != params.procedure.bandwidthTime)
    {
        sem_post( &UDPSem);
        usleep(1000);
        interval++;
    }
    //usleep(1000000);
    sleep(2); 
    pthread_cancel(sendUDPBW_thread);
    //printf("Paquetes enviados: %d\n", sequenceNumber);
    if (ok)
    {
        // Calculate PacketLoss
        bool okCalculated = calculateBandwidthPacketLossStage1(mReceivedMessagesUDP, results.values.packetLoss, params.procedure.bandwidthTime, results.values.bandwidth);
        #if SHOW_INFO
            if (!okCalculated)
            {
                printf( "PacketLoss Calculation Error");
            }
            printf( "MEASURING RESULT - BandWidth Down: %.3f kb/s\n", results.values.bandwidth );
            printf( "MEASURING RESULT - PacketLoss Down: %.3f %\n", results.values.packetLoss );
        #endif
    }
    ok &= !mReceivedMessagesTCP.readCancelMessage();

    if(!ok)
    {
        printf( "ERROR:sendUdpData BWidth.\n" );
    }

    if (ok)
    {
        ok &= interchangeMeasurementProcedure(upMeasurements, results);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Bandwidth Up: %.3f kb/s\n", upMeasurements.bandwidth);
            printf( "MEASURING RESULT - PacketLoss Up: %.3f %\n", upMeasurements.packetLoss );
        #endif
    }

    if (ok)
    {
        // Check stage 1
        upResults.values = upMeasurements;
        ok &= checkStage1(params.bandWidthUp, params.packetLossUp, params.bandWidthDown, params.packetLossDown, upResults, results);
    }

    if (!ok)
    {
        showCheckMessage(upResults, results);
    }
    
    return ok;
}


void* Q4SServerProtocol::manageTcpConnectionsFn( void* lpData )
{
    Q4SServerProtocol* q4sCFI = ( Q4SServerProtocol* )lpData;
    return q4sCFI->manageTcpConnection( );
}

void* Q4SServerProtocol::manageUdpReceivedDataFn( void* lpData )
{
    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}
void* Q4SServerProtocol::manageTcpConnection( )
{
    bool        ok = true;
    int         newConnId = 1;
    int         thread_error;
    if( ok )
    {
        ok &= mServerSocket.startTcpListening( );
    }

    while( ok )
    {
        ok &= mServerSocket.waitForTcpConnection( newConnId );
        //printf("CONEXION TCP\n");
        if( ok )
        {
            ManageTcpConnectionsFnInfo  fnInfo;
            fnInfo.pThis = this;
            fnInfo.connId = newConnId;
            thread_error = pthread_create( &marrthrDataHandle[0], NULL, manageTcpReceivedDataFn, ( ManageTcpConnectionsFnInfo* ) &fnInfo);
            if (thread_error< 0)
            {
                printf("ERRor marrthrDataHandle\n");
            }
        }
        //newConnId++;
    }
    /*
    pthread_mutex_lock (&mut_stop);
    stop=true; 
    pthread_mutex_unlock (&mut_stop);
    */
    //Q4SServerProtocol::done();  
}

void* Q4SServerProtocol::manageTcpReceivedDataFn( void* lpData )
{
    ManageTcpConnectionsFnInfo* q4sCFI = ( ManageTcpConnectionsFnInfo* )lpData;
    return q4sCFI->pThis->manageTcpReceivedData( q4sCFI->connId );
}

void* Q4SServerProtocol::manageTcpReceivedData( int connId )
{
    bool                ok = true;
    char                buffer[ 65536 ]={};    
    struct timeval time_s;
    int time_error;            
    while( ok ) 
    {
        ok &= mServerSocket.receiveTcpData( connId, buffer, sizeof( buffer ) );
        if( ok )
        {
            std::string message = buffer;
            mReceivedMessagesTCP.addMessage ( message );
        }
    }
//printf("FIN HILO RECEPCION TCP \n");
}

void* Q4SServerProtocol::manageUdpReceivedData( )
{
    bool                ok = true;
    char                udpBuffer[ 65536 ];
    int                 connId;
    struct timeval time_s;
    int time_error; 
    uint64_t actualTimeStamp;
    std::string message;
    int pingNumber = 0;
    uint64_t receivedTimeStamp = 0;
    mServerSocket.startUdpListening( );
    memset(udpBuffer, 0, sizeof(udpBuffer));

    while ( ok )
    {            
        memset(udpBuffer, 0, sizeof(udpBuffer));
        ok &= mServerSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ), connId );

        if( ok )
        {
            time_error = gettimeofday(&time_s, NULL); 
            actualTimeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
            char reasonPhrase[ 256 ]={};
            message = std::string(udpBuffer);
            pingNumber = 0;
            receivedTimeStamp = 0;

            // Comprobar que es un ping
            if ( Q4SMessageTools_isPingMessage(udpBuffer, &pingNumber, &receivedTimeStamp) )
            {
                #if SHOW_INFO2
                    printf( "Received Ping, number:%d, timeStamp: %"PRIu64"\n", pingNumber, receivedTimeStamp);
                #endif

                // mandar respuesta del ping
                reasonPhrase[ 256 ]={0};
                #if SHOW_INFO2
                    printf( "Ping responsed %d\n", pingNumber);
                #endif
                Q4SMessage message200;
                sprintf( reasonPhrase, "Q4S/1.0 200 OK\nSequence-Number: %d\nTimestamp: %"PRIu64"\n", pingNumber,receivedTimeStamp );

                ok &= mServerSocket.sendUdpBWData( connId, reasonPhrase );

                #if SHOW_INFO2
                    printf( "Received Udp: <%s>\n", udpBuffer );
                #endif
               // encolar el ping y el timestamp para el calculo del jitter
            }
            mReceivedMessagesUDP.addMessage(message, actualTimeStamp);           
        }
    }
}

void* Q4SServerProtocol::sendUDPBWFn(void* lpData )
{
    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;    
    //unsigned long bandWidthDown=*(); 

    bool ret = q4sCP->sendUDPBW( q4sCP->bandWidthUp);
}
 void *Q4SServerProtocol::sendUDPBW(unsigned long bandWidthUp)
 {   
    int time_error; 
    bool ok= true; 
    //unsigned long bandWidthDown=*((unsigned long*)bandWidthDownParam); 
    float message_size= 1000*8; 
    float bandWidthUpInc= (float)bandWidthUp*1.16; // Se multiplica por 1.05 para dejar un margen superior 

    float messages_fract_per_ms = (bandWidthUpInc/ (float) message_size);
    int   messages_int_per_ms = floor(messages_fract_per_ms);

    float messages_per_s[10];
    messages_per_s[0] = ((messages_fract_per_ms - (float) messages_int_per_ms) * 1000);
    int ms_per_message[11]={};
    ms_per_message[0] = 1;
    int divisor;

    for (int i = 0; i < 10; i++) 
    {
        divisor = 2;
        ////////////////////////////////////////////////
        // MAYOR O IGUAL 
        ////////////////////////////////////////////////
        //printf("message per s: %f\n", messages_per_s[i]);
        while ((1000/divisor) >= messages_per_s[i]) 
        {
            divisor++;
        }
        ms_per_message[i+1] = divisor;
        if (messages_per_s[i] - ((float)1000/divisor) == 0) 
        {
            //printf("multiplos 1: %d\n", ms_per_message[i+1]);

            break;
        } 
        else if (messages_per_s[i] - ((float)1000/divisor) <= 1) 
        {

            ms_per_message[i+1]--;
            //printf("multiplos 2: %d\n", ms_per_message[i+1]);

            break;
        } 
        else 
        {        
            //printf("multiplos 3: %d\n", ms_per_message[i+1]);
            messages_per_s[i+1] = messages_per_s[i] -((float)1000/divisor);
        }
    } 

    Q4SMessage message;
    char message_char[2048] = {0};
    struct timeval time_t_aux;
    time_error = gettimeofday(&time_t_aux, NULL); 
    uint64_t TimeStamp2;     
    uint64_t TimeStamp3; 
    uint64_t diffTime; 

    unsigned long sequenceNumber = 0;
    uint64_t initialTimeStamp =  time_t_aux.tv_sec*1000 + time_t_aux.tv_usec/1000;   
    int interval= 0;
    while(1)
    {
        sem_wait( &UDPSem);
        int j = 0; 
        
        time_error = gettimeofday(&time_t_aux, NULL); 
        TimeStamp2 =  time_t_aux.tv_sec*1000 + time_t_aux.tv_usec/1000;
        //printf("%lu\n", TimeStamp2);
        while (j < messages_int_per_ms) 
            {
                //ok &= message.initRequest(Q4SMTYPE_BWIDTH, "myIp", q4SServerConfigFile.defaultUDPPort, true, sequenceNumber, true, TimeStamp2);
                sprintf(message_char,
            "BWIDTH q4s://myIp:27016  Q4S/1.0\nSequence-Number:%d\nTimestamp:%"PRIu64"\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
            sequenceNumber,TimeStamp2);
                ok &= mServerSocket.sendUdpData(DEFAULT_CONN_ID,   message_char);
                sequenceNumber++; 
                j++; 
            }

       for (int k = 1; k < 11; k++) 
        {
            if (ms_per_message[k] > 0 && interval % ms_per_message[k] == 0) 
            {        
                //printf("mensahe extra %d, %d, %d, %d\n", interval, ms_per_message[k], k, sizeof(ms_per_message));
                sprintf(message_char,
            "BWIDTH q4s://myIp:27016  Q4S/1.0\nSequence-Number:%d\nTimestamp:%"PRIu64"\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
            sequenceNumber,TimeStamp2);
                ok &= mServerSocket.sendUdpData(DEFAULT_CONN_ID,   message_char);
                
                sequenceNumber++;  
            }
        }
        //printf("%lu\n", TimeStamp2);
        //printf("sequenceNumber: %d\n", sequenceNumber); 
        /* 
        time_error = gettimeofday(&time_t_aux, NULL);
        TimeStamp3 =  time_t_aux.tv_sec*1000 + time_t_aux.tv_usec/1000;
        printf("TimeStamp3 %lu\t",TimeStamp3);
        diffTime= (int)(TimeStamp3- TimeStamp2);    
        printf("diffTime %d\n", diffTime); 
        //usleep(1000-(diffTime*1000)); 
        */

        interval++; 
    } 
 }