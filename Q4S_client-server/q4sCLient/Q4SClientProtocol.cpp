#include "Q4SClientProtocol.h"
#include "errno.h"
#include <stdio.h>
#include <sstream>
#include <math.h>

#include <sys/time.h>
//#include "ETime.h"
#include "../q4sCommon/Q4SMathUtils.h"
#include "Q4SClientConfigFile.h"
//#include "EKey.h"
#include "../q4sCommon/Q4SMessage.h"
#include "../q4sCommon/Q4SMessageTools.h"
static const char alphanum[] ="0123456789!@#$%^&*ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
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
    #if SAVE_INFO
    remove( "../../test/measured/measure_client.txt" ); 
    #endif
    if (ok)
    {
        ok &= mReceivedMessagesUDP.init( );
        ok &= mReceivedMessagesTCP.init( );

    }

    if (ok)
    {
        ok &= tryOpenConnectionsTimes(times, milisecondsBetweenTimes);
    }
    if (ok)
    {
        sem_init(&UDPSem, 0, 0);

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
    mReceivedMessagesTCP.done( );
    mReceivedMessagesUDP.done( );
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
    //printf("METHOD: bwidth TODO\n");
}
void Q4SClientProtocol::cancel()
{
    #if SHOW_INFO
        printf("METHOD: cancel\n");
    #endif
    mClientSocket.sendTcpData( "CANCEL\r\n" );
    //done(); 
}

bool Q4SClientProtocol::ready(unsigned long stage,Q4SSDPParams &params)
{
   // printf("METHOD: ready\n");

    bool ok = true;        
    if( ok )
    {

        Q4SMessage message;
        message.initRequest(Q4SMTYPE_READY, "myIP", q4SClientConfigFile.defaultTCPPort, false, 0, false, 0, true, stage);
        while(mReceivedMessagesUDP.size()>0)
        {
            //printf("BORRANDO MENSAJES\n");
            mReceivedMessagesUDP.eraseMessages();
        }
        ok &= mClientSocket.sendTcpData( message.getMessageCChar() );
    }
    /*
    if (stage==0)
    {
        int thread_error=  pthread_cancel(marrthrHandle[0]);
        thread_error = pthread_create( &marrthrHandle[0], NULL, manageUdpReceivedDataFn, ( void* ) this);
    }

    if (stage==1)
    {
        int thread_error=  pthread_cancel(marrthrHandle[0]);
        thread_error = pthread_create( &marrthrHandle[0], NULL, manageBWReceivedDataFn, ( void* ) this);
    }
    */
    if ( ok )
    {
        std::string message;
        mReceivedMessagesTCP.readFirst( message );
        //printf("%s\n", message.c_str());
       
        ok &= Q4SMessageTools_is200OKMessage(message, false, 0, 0); 
        while (!ok)
        {
            mReceivedMessagesTCP.readFirst( message );
            ok = Q4SMessageTools_is200OKMessage(message,false, 0, 0);
        }
        ok &= Q4SSDP_parse(message, params);
        qosLevel= params.qosLevelDown;
    }

    return ok;
}

bool Q4SClientProtocol::handshake(Q4SSDPParams &params)
{
    //printf("----------Handshake Phase\n");
    //printf("METHOD: begin\n");

    bool ok = true;
    
    if( ok )
    {
        //sleep(5);
        Q4SMessage message;
        message.initRequest(Q4SMTYPE_BEGIN, "myIP", q4SClientConfigFile.defaultTCPPort);
        ok &= mClientSocket.sendTcpData( message.getMessageCChar() );
    }

    if ( ok ) 
    {
        std::string message;

        mReceivedMessagesTCP.readFirst( message );            
        ok = Q4SMessageTools_is200OKMessage(message,false, 0, 0);

        while (!ok)
        {
            mReceivedMessagesTCP.readFirst( message );
            ok = Q4SMessageTools_is200OKMessage(message,false, 0, 0);
        }
        ok &= Q4SSDP_parse(message, params);
        qosLevel= params.qosLevelDown;
    }

    return ok;
}
bool Q4SClientProtocol::negotiation(Q4SSDPParams params, Q4SMeasurementResult &results)
{
    //printf("----------Negotiation Phase\n");
    bool ok= true; 
    bool measureOk= false;     
    int QOS_negotiation;
    Q4SMeasurementResult downResults;
    char data2save[200]={}; 
    char command_curl[300]={}; 
    unsigned long auxBW;
    int pck_size; 
    for( QOS_negotiation = 0; (measureOk  == false ) && ( QOS_negotiation!= 11 ); QOS_negotiation++ )
    {

        ok &= Q4SClientProtocol::ready(0, params);
        if( ok )
        {
            //printf("MEASURING\n");
            
            measureOk = Q4SClientProtocol::measureStage0(params, results, downResults, params.procedure.windowSizeLatencyCalcDownlink);
            if (measureOk)
            {
                ok &= Q4SClientProtocol::ready(q4SClientConfigFile.ready_BW,params);
                if (ok && q4SClientConfigFile.ready_BW==1)
                {
                    measureOk = Q4SClientProtocol::measureStage1(params, results, downResults);
                }
            }
        }

        if (measureOk)
        {
            mResults = results;
        }
     #if SAVE_INFO
        struct timeval time_s;
        std::ofstream pFile("../../test/measured/measure_client.txt", ios::out | ios::app);; 
        int time_error = gettimeofday(&time_s, NULL); 
        uint64_t actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
        sprintf(data2save, "0\t%" PRIu64 "\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%d\t%d\n",actualTime,results.values.latency,downResults.values.latency, results.values.jitter, downResults.values.jitter,results.values.bandwidth, downResults.values.bandwidth,results.values.packetLoss, downResults.values.packetLoss,QOS_negotiation, measureOk); 
        pFile<<data2save;
    #endif
    #if PLOT_INFO
        sprintf(command_curl, "curl -i -XPOST 'http://192.168.1.102:8086/write?db=racing_drones&precision=ms' --data-binary 'q4s_client latency_up=%.3f,latency_down=%.3f,jitter_up=%.3f,jitter_down=%.3f,BW_up=%.3f,BW_down=%.3f,loss_up=%.3f,loss_down=%.3f'",results.values.latency,downResults.values.latency, results.values.jitter, downResults.values.jitter,results.values.bandwidth, downResults.values.bandwidth,results.values.packetLoss, downResults.values.packetLoss);
        system(command_curl);
        
    #endif
    }    
    //printf("SALIENDO DE NEGOTIATION: %d\n", measureOk);
    qosLevel= params.qosLevelUp;
    qosLevelMax= qosLevel; 
    if (!measureOk)
    {
        usleep(params.alertPause*1000);

        ok = false;  
    }
    return ok;
}

void Q4SClientProtocol::continuity(Q4SSDPParams params)
{

//printf("----------Continuity Phase\n");

    bool stop = false;
    bool measureOk = true;
    char data2save[200]={}; 
    char command_curl[300]={}; 
    Q4SMeasurementResult results;
    Q4SMeasurementResult downResults;
    while ( !stop )
    {
        //printf("MEASURING\n");



        measureOk = Q4SClientProtocol::measureContinuity(params, results, downResults, params.procedure.windowSizeLatencyCalcDownlink);

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

            //printf(alertMessage.c_str());

            if (qosLevel==10)
            {
                //printf("QOS: %d\n", qosLevel);
                stop= true; 
            }
            if (!stop)
            {
                alert(params); 
            }
            showCheckMessage(results, downResults);
    
        }
        else
        {
            recovery(params); 
        }
    #if SAVE_INFO
        struct timeval time_s;
        std::ofstream pFile("../../test/measured/measure_client.txt", ios::out | ios::app); 
        //pFile = fopen ("measure_client.txt","w");        
        int time_error = gettimeofday(&time_s, NULL); 
        uint64_t actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
        sprintf(data2save, "1\t%" PRIu64 "\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%d\t%d\n",actualTime,results.values.latency,downResults.values.latency, results.values.jitter, downResults.values.jitter,results.values.bandwidth, downResults.values.bandwidth,results.values.packetLoss, downResults.values.packetLoss,qosLevel, measureOk); 
        pFile<<data2save;
        //fclose(pFile); 
    #endif
    #if PLOT_INFO
        sprintf(command_curl, "curl -i -XPOST 'http://192.168.1.102:8086/write?db=racing_drones&precision=ms' --data-binary 'q4s_client latency_up=%.3f,latency_down=%.3f,jitter_up=%.3f,jitter_down=%.3f,loss_up=%.3f,loss_down=%.3f'",results.values.latency,downResults.values.latency, results.values.jitter, downResults.values.jitter,results.values.packetLoss, downResults.values.packetLoss);
        system(command_curl);
        
    #endif

    }
}

void Q4SClientProtocol::recovery(Q4SSDPParams params)
{
    if (qosLevel ==  qosLevelMax)
    {
       //printf("No recovery send because QOS Level = %d\n", qosLevelMax);
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
       if ( timeForRecovery > params.recoveryPause)
       {
           qosLevel--;

           recoveryTimeStamp = actualTime;
           //printf("METHOD: recovery\n");
           //printf("QOS Level: %d\n", qosLevel);
       }
    }
}

void Q4SClientProtocol::alert(Q4SSDPParams params)
{   
    struct timeval time_s;
    int time_error = gettimeofday(&time_s, NULL); 

    uint64_t actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
    uint64_t timeFromLastAlert = actualTime - lastAlertTimeStamp;
    if ( timeFromLastAlert > params.alertPause)
    {
        qosLevel++;
        lastAlertTimeStamp = actualTime;


        //printf("METHOD: alert\n");
        
    }
}

bool Q4SClientProtocol::measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend)
{
    bool ok = true;

    std::vector<uint64_t> arrSentPingTimestamps;
    Q4SMeasurementValues downMeasurements;
    char data2save[200]={}; 
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
            mReceivedMessagesUDP, 
            arrSentPingTimestamps, 
            results.values.latency, 
            pingsToSend, 
            q4SClientConfigFile.showMeasureInfo);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Latency Up:%.3f ms\n", results.values.latency );
        #endif
        // Calculate Jitter
        float packetLoss = 0.f;
        calculateJitterStage0(
            mReceivedMessagesUDP, 
            results.values.jitter,
            params.procedure.negotiationTimeBetweenPingsUplink, 
            pingsToSend,
            q4SClientConfigFile.showMeasureInfo);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Jitter Up: %.3f ms\n", results.values.jitter );
        #endif
    }

    if ( ok ) 
    {

        results.values.packetLoss= 0; 
        results.values.bandwidth= 0; 
        ok &= interchangeMeasurementProcedure(downMeasurements, results);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Latency Down: %.3f ms\n", downMeasurements.latency );
            printf( "MEASURING RESULT - Jitter Down: %.3f ms\n", downMeasurements.jitter );
        #endif
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
    while(mReceivedMessagesUDP.size()>0)
    {
        //printf("BORRANDO MENSAJES\n");
        mReceivedMessagesUDP.eraseMessages();
    }
    if ( ok ) 
    {
        // Send Info Ping with sequenceNumber 0
        Q4SMessage infoPingMessage;
        ok &= infoPingMessage.initPing("myIp", q4SClientConfigFile.defaultUDPPort, 0, 0, true, &results.values);
        ok &= mClientSocket.sendTcpData( infoPingMessage.getMessageCChar() );
    }
    
    if (ok)
    {
        // Wait to recive the measurements Ping
        Q4SMessageInfo  messageInfo={};
        ok= false; 
        while(!ok)
        {

            ok = mReceivedMessagesTCP.readPingMessage( 0, messageInfo, true );
            if (ok)
            {
                ok &= Q4SMeasurementValues_parse(messageInfo.message, downMeasurements);
                if (!ok)
                {
                    printf( "ERROR:Interchange Read measurements fail\n");
                }
            }
          
        }
    }

    return ok;
}

bool Q4SClientProtocol::measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend)
{
    bool ok = true;

    std::vector<uint64_t> arrSentPingTimestamps;
    Q4SMeasurementValues downMeasurements;
    bool okCancel= true; 
    struct timeval time_s;   
    int time_error = gettimeofday(&time_s, NULL); 
    uint64_t initialTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
    uint64_t actualTime = initialTime; 

    if (ok)
    {
        // Send regular pings
        ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.continuityTimeBetweenPingsUplink);
    }
    #if SHOW_INFO
        if(!ok)
        {
            printf( "ERROR:sendUdpData PING.\n" );
        }
    #endif
    if (ok)
    {
        while(okCancel && mReceivedMessagesUDP.size()==0)
        {
            time_error = gettimeofday(&time_s, NULL);  
            actualTime =  time_s.tv_sec*1000 + time_s.tv_usec/1000;   
            okCancel &= (actualTime<(initialTime+5000)); 

        }
        usleep(params.procedure.negotiationTimeBetweenPingsUplink *1000);

        // Calculate Latency
        calculateLatency(
            mReceivedMessagesUDP, 
            arrSentPingTimestamps, 
            results.values.latency, 
            pingsToSend, 
            q4SClientConfigFile.showMeasureInfo);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Latency Up: %.3f ms\n", results.values.latency );
        #endif
        // Calculate Jitter
        calculateJitterAndPacketLossContinuity(
            mReceivedMessagesUDP, 
            results.values.jitter, 
            params.procedure.continuityTimeBetweenPingsUplink, 
            pingsToSend,
            results.values.packetLoss,
            q4SClientConfigFile.showMeasureInfo);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Jitter Up: %.3f ms\n", results.values.jitter );
            printf( "MEASURING RESULT - PacketLoss Up: %.3f %%\n", results.values.packetLoss );
        #endif
    }

    if (ok)
    {
        results.values.bandwidth= 0; 
        ok &= interchangeMeasurementProcedure(downMeasurements, results);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Latency Down: %.3f ms\n", downMeasurements.latency );
            printf( "MEASURING RESULT - Jitter Down: %.3f ms\n", downMeasurements.jitter );
            printf( "MEASURING RESULT - PacketLoss Down: %.3f %%\n", downMeasurements.packetLoss );
        #endif
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

bool Q4SClientProtocol::sendRegularPings(std::vector<uint64_t> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings)
{
    bool ok = true;

    Q4SMessage message;
    uint64_t timeStamp = 0;
    int pingNumber = 0;
    int pingNumberToSend = pingsToSend;
    int time_error ;
    struct timeval time_s;
    for ( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
    {
        // Store the timestamp
        time_error = gettimeofday(&time_s, NULL); 
        timeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
        //timeStamp = (unsigned long) 20; 
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
    
    //const unsigned long* bandWidthDown = (long unsigned int*)malloc(sizeof(bandWidthDown)); 
    bandWidthDown = params.bandWidthDown;
    Q4SMeasurementValues downMeasurements;
    size_packet= params.size_packet;

    pthread_create(&sendUDPBW_thread, NULL, sendUDPBWFn, ( void* ) this);

    //printf( "Starting:measureStage1.\n" );    

    

    Q4SMessage message;
    struct timeval time_s;
    int time_error = gettimeofday(&time_s, NULL); 
    struct timeval time_t_aux;
    time_error = gettimeofday(&time_t_aux, NULL); 
    uint64_t TimeStamp3; 
    uint64_t diffTime; 

    unsigned long sequenceNumber = 0;
    int interval= 0;
    while(ok && interval != params.procedure.bandwidthTime)
    {
        sem_post( &UDPSem);
        usleep(1000);//Tiene un error aproximado del 10-15%
        interval++;
    }
time_error = gettimeofday(&time_t_aux, NULL); 
TimeStamp3 =  time_t_aux.tv_sec*1000 + time_t_aux.tv_usec/1000;    
diffTime= (time_t_aux.tv_sec - time_s.tv_sec)*1e6 + (time_t_aux.tv_usec - time_s.tv_usec);    

    //sleep(5); 
    pthread_cancel(sendUDPBW_thread) ; 
    
    //printf("TimeStamp3:%"PRIu64"\tdiffTime %"PRIu64"\n",TimeStamp3, diffTime);
    sleep(2);

    
    //printf("Sequence Number: %d interval: %d\n", sequenceNumber, interval);
    if (ok)
    {
        // Calculate PacketLoss
        bool okCalculated = calculateBandwidthPacketLossStage1(mReceivedMessagesUDP, results.values.packetLoss, params.procedure.bandwidthTime, results.values.bandwidth);
        #if SHOW_INFO
            if (!okCalculated)
            {
                printf( "PacketLoss Calculation Error");
            }        
            

            printf( "MEASURING RESULT - BandWidth Up: %.3f kb/s\n", results.values.bandwidth );

            printf( "MEASURING RESULT - PacketLoss Up: %.3f %%\n", results.values.packetLoss );
        #endif
    }
    #if SHOW_INFO
        if(!ok)
        {
            printf( "ERROR:sendUdpData BWidth.\n" );
        }
    #endif
    if (ok)
    {
        ok &= interchangeMeasurementProcedure(downMeasurements, results);
        #if SHOW_INFO
            printf( "MEASURING RESULT - Bandwidth Down: %.3f Kb/s\n", downMeasurements.bandwidth);
            printf( "MEASURING RESULT - PacketLoss Down: %.3f %%\n", downMeasurements.packetLoss );
        #endif

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
    char            udpBuffer[ 2048 ]={};
    int             time_error;
    int             pingNumber; 
    uint64_t        actualTimeStamp;
    uint64_t        receivedTimeStamp;
    struct          timeval time_s;
    int contadorpaquetes= 0; 
    std::string message;
    memset(udpBuffer, 0, sizeof(udpBuffer));
    while ( ok )
    {
        memset(udpBuffer, 0, sizeof(udpBuffer));

        ok &= mClientSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ) );
        //printf("%s\n", udpBuffer);
        contadorpaquetes ++; 
        if( ok )
        {
            time_error = gettimeofday(&time_s, NULL); 
            actualTimeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
            //actualTimeStamp = (unsigned long)22; 
            message = udpBuffer;
           

            pingNumber = 0;

            // Comprobar que es un ping
            if ( Q4SMessageTools_isPingMessage(udpBuffer, &pingNumber, &receivedTimeStamp) )
            {

                #if SHOW_INFO2                
                    printf( "Received Ping, number:%d, timeStamp: %" PRIu64 "\n", pingNumber, receivedTimeStamp);
                #endif

                // mandar respuesta del ping
                char reasonPhrase[ 256 ];
                #if SHOW_INFO2
                    printf( "Ping responsed %d\n", pingNumber);
                #endif
                Q4SMessage message200;
                //printf(" RECEIVED TIMESTAMP: %lu TIMESTAMP MESSAGE: %lu\n", receivedTimeStamp, );
                sprintf( reasonPhrase, "Q4S/1.0 200 OK\r\nSequence-Number:%d\r\nTimestamp:%" PRIu64 "\r\nContent-Length:0\r\n\r\n", pingNumber,receivedTimeStamp );
                ok &= mClientSocket.sendUdpData( reasonPhrase );

                #if SHOW_INFO2
                    printf( "Received Udp: <%s>\n", udpBuffer );
                #endif
            }
            //printf("ACTUAL TIMESTAMP: %lu\n MESSAGE:%s\n***********************************************\n", actualTimeStamp, message.c_str());
            mReceivedMessagesUDP.addMessage(message, actualTimeStamp);

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
    //printf("%d\n", contadorpaquetes);
}
void* Q4SClientProtocol::manageBWReceivedDataFn( void* lpData )
{
    Q4SClientProtocol* q4sCP = ( Q4SClientProtocol* )lpData;
    bool ret = q4sCP->manageBWReceivedData( );
    
}

void* Q4SClientProtocol::manageBWReceivedData( )
{
    bool            ok = true;
    char            udpBuffer[ 2048 ];
    int             time_error;
    int             pingNumber; 
    uint64_t   actualTimeStamp;
    uint64_t   receivedTimeStamp;
    struct          timeval time_s,time_s2,time_s0;
    int contadorpaquetes= 0; 
    std::string message;

    while ( ok )
    {

//gettimeofday(&time_s0, NULL); 
        ok &= mClientSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ) );
//gettimeofday(&time_s2, NULL); 
//printf("********************************\nPacket: %d\nCost receiveUdpData: %lus%luus\n",contadorpaquetes,(time_s2.tv_sec-time_s0.tv_sec),(time_s2.tv_usec-time_s0.tv_usec));

        contadorpaquetes ++; 
        if( ok )
        {
            time_error = gettimeofday(&time_s, NULL); 
            actualTimeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
            message = std::string(udpBuffer);

            pingNumber = 0;

            // Comprobar que es un ping
            
//gettimeofday(&time_s, NULL); 

            mReceivedMessagesUDP.addMessage(message, actualTimeStamp);

//gettimeofday(&time_s2, NULL); 
//printf("Cost addMessage: %lus%luus\nTotal:%lus%luus\n********************************\n",(time_s2.tv_sec-time_s.tv_sec),(time_s2.tv_usec-time_s.tv_usec),(time_s2.tv_sec-time_s0.tv_sec),(time_s2.tv_usec-time_s0.tv_usec));
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
    //printf("%d\n", contadorpaquetes);
}
void* Q4SClientProtocol::manageTcpReceivedData( )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    memset(buffer, 0, sizeof(buffer));

    while (ok) 
    {    
        
        memset(buffer, 0, sizeof(buffer));
        ok &= mClientSocket.receiveTcpData( buffer, sizeof( buffer ) );
        if( ok )
        {
            //printf("%s\n", &buffer);
            std::string message = buffer;
            mReceivedMessagesTCP.addMessage ( message );
        }
    }

}
void* Q4SClientProtocol::sendUDPBWFn(void* lpData )
{
    Q4SClientProtocol* q4sCP = ( Q4SClientProtocol* )lpData;    
    //unsigned long bandWidthDown=*(); 

    bool ret = q4sCP->sendUDPBW(q4sCP->bandWidthDown, q4sCP->size_packet);
}
 void *Q4SClientProtocol::sendUDPBW(unsigned long bandWidthDown, int size_packet)
 {   
    char message_char[2048] = {0};
    bool ok= true; 
    //unsigned long bandWidthDown= (unsigned long) 100000; 
    //unsigned long bandWidthDown=*((unsigned long*)bandWidthDownParam); 
    float message_size= size_packet*8; 
    float bandWidthDownInc= (float)bandWidthDown*1.16; // Se multiplica por 1.16 para dejar un margen superior 

    float messages_fract_per_ms = (bandWidthDownInc/ (float) message_size);
    //printf("%f\n", messages_fract_per_ms);
    int   messages_int_per_ms = floor(messages_fract_per_ms);

    float messages_per_s[10];
    messages_per_s[0] = ((messages_fract_per_ms - (float) messages_int_per_ms) * 1000);
    int ms_per_message[11]={0};
    ms_per_message[0] = 1;
    int divisor;
    char msn_BW[size_packet]; 
    for(int z=0; z < size_packet; z++)
    {
        
        msn_BW[z]= alphanum[rand() % (sizeof(alphanum)-1)];

    }
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
    struct timeval time_s, time_s0, time_s1;
    int time_error = gettimeofday(&time_s, NULL); 
    struct timeval time_t_aux;
    time_error = gettimeofday(&time_t_aux, NULL); 
    uint64_t TimeStamp2;     
    //uint64_t TimeStamp3; 
    //uint64_t diffTime; 

    unsigned long sequenceNumber = 0;
    int interval= 0;

    while(1)
    {
        sem_wait( &UDPSem);
        int j = 0; 
        
        time_error = gettimeofday(&time_t_aux, NULL); 
        TimeStamp2 =  time_t_aux.tv_sec*1000 + time_t_aux.tv_usec/1000;



        
        while (j < messages_int_per_ms) 
        {
//printf("********************************\n");
//gettimeofday(&time_s0, NULL); 
                //ok &= message.initRequest(Q4SMTYPE_BWIDTH, "myIp", q4SClientConfigFile.defaultUDPPort, true, sequenceNumber, true, TimeStamp2);
             sprintf(message_char,
            "BWIDTH q4s://myIp:27016  Q4S/1.0\r\nSequence-Number:%lu\r\nTimestamp:%" PRIu64 "\r\n\r\n%s",sequenceNumber,TimeStamp2, msn_BW);
//gettimeofday(&time_s1, NULL); 
//printf("Cost initRequest: %lus%luus\n",(time_s1.tv_sec-time_s0.tv_sec),(time_s1.tv_usec-time_s0.tv_usec));
//gettimeofday(&time_s0, NULL); 
            ok &= mClientSocket.sendUdpBWData(message_char);
//gettimeofday(&time_s1, NULL); 
//printf("Cost sendUdpData: %lus%luus\n",(time_s1.tv_sec-time_s0.tv_sec),(time_s1.tv_usec-time_s0.tv_usec));

            sequenceNumber++; 
            j++; 
        }

       for (int k = 1; k < 11; k++) 
        {
            if (ms_per_message[k] > 0 && interval % ms_per_message[k] == 0) 
            {        
                //printf("mensahe extra %d, %d, %d, %d\n", interval, ms_per_message[k], k, sizeof(ms_per_message));
                 sprintf(message_char,
            "BWIDTH q4s://myIp:27016  Q4S/1.0\nSequence-Number:%lu\nTimestamp:%" PRIu64 "\r\n\r\n%s",sequenceNumber,TimeStamp2, msn_BW);
                //ok &= message.initRequest(Q4SMTYPE_BWIDTH, "myIp", q4SClientConfigFile.defaultUDPPort, true, sequenceNumber, true, TimeStamp2);
                ok &= mClientSocket.sendUdpBWData(message_char);
                sequenceNumber++;  
            }
        }
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