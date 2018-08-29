#include "Q4SCommonProtocol.h"
#include <sys/time.h>
#include <stdio.h>
#include "Q4SMessageInfo.h"
#include "Q4SMessageManager.h"
#include "Q4SMathUtils.h"
#include <sstream>



void Q4SCommonProtocol::calculateLatency(Q4SMessageManager &mReceivedMessages, std::vector<uint64_t> &arrSentPingTimestamps, float &latency, unsigned long pingsSent, bool showMeasureInfo)
{
    Q4SMessageInfo messageInfo;
    std::vector<float> arrPingLatencies;
    float actualPingLatency;
    int pingIndex = 0;
    int pingMaxCount = pingsSent;
    bool ok= true; 
    int sequenceNumberPing; 
    uint64_t TimeStampPing; 
    // Prepare for Latency calculation
    for( pingIndex = 0; pingIndex < pingMaxCount && mReceivedMessages.size()>0; pingIndex++ )
    {
        if( mReceivedMessages.read200OKMessage( messageInfo, true, &TimeStampPing, &sequenceNumberPing ) == true )
        {
            // Actual ping latency calculation
            //printf("TIEMPO 200 0k: %"PRIu64"\nTIEMPO PING: %"PRIu64"\n",messageInfo.timeStamp, TimeStampPing);

            
            actualPingLatency = (messageInfo.timeStamp - TimeStampPing)/ 2.0f;
            //printf("TIEMPO 200 0k: %lu\nTIEMPO PING: %lu\n",messageInfo.timeStamp, arrSentPingTimestamps[ pingIndex ]);
            // Latency store
            arrPingLatencies.push_back( actualPingLatency );

            #if SHOW_INFO2
            
                printf( "PING %d actual ping latency: %.3f\n", pingIndex, actualPingLatency );
            #endif
        }
        else
        {
            #if SHOW_INFO2
                printf( "PING RESPONSE %d message lost\n", pingIndex);
            #endif
        }
    }

    // Latency calculation
    latency = EMathUtils_median( arrPingLatencies );
}

void Q4SCommonProtocol::calculateJitter(
    Q4SMessageManager &mReceivedMessages, 
    float &jitter, 
    unsigned long timeBetweenPings, 
    unsigned long pingsSent, 
    bool calculatePacketLoss, 
    float &packetLoss, 
    bool showMeasureInfo) 
{
    int pingIndex = 0;
    int pingMaxCount = pingsSent;
    Q4SMessageInfo messageInfo;
    std::vector<uint64_t> arrReceivedPingTimestamps;
    std::vector<unsigned long> arrPingJitters={};
    bool firstPing = true; 
    uint64_t actualPingTimeWithPrevious;
    unsigned long packetLossCount = 0;
    int indexReceived= 0; 
    struct timeval time_s;
    int time_error = gettimeofday(&time_s, NULL); 
    uint64_t timeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
    // Prepare for Jitter calculation
    //printf("TIMESTAMP: %"PRIu64"\n", timeStamp);
    for( pingIndex = 0; pingIndex < pingMaxCount && mReceivedMessages.size()>0; pingIndex++ )
    {
        time_error = gettimeofday(&time_s, NULL); 
        timeStamp =  time_s.tv_sec*1000 + time_s.tv_usec/1000;
        //printf("TIMESTAMP: %"PRIu64"\n", timeStamp);
        if( mReceivedMessages.readPingMessage( pingIndex, messageInfo, true ) == true )
        {
            //printf("*********************************************************************\n");
            //printf("index jitter: %d\n", pingIndex);

            arrReceivedPingTimestamps.push_back( messageInfo.timeStamp );
            //printf("MENSAJE LEIDO%s\n", messageInfo.message.c_str());
            //printf("%"PRIu64"\n",messageInfo.timeStamp  );
            if( !firstPing)
            {
                // Actual time between this ping and previous calculation
                /*
                if (arrReceivedPingTimestamps[ indexReceived ] < arrReceivedPingTimestamps[ indexReceived - 1 ])
                {
                    printf("///////////////////////////////////////////////////////////////////////////////////////////");
                    printf("ERRORR!!!!\n printf(pingIndex: %d     indexReceived:%lu       indexReceived anterior:%lu\n", pingIndex, arrReceivedPingTimestamps[ indexReceived ], arrReceivedPingTimestamps[ indexReceived - 1 ] );
                    printf("///////////////////////////////////////////////////////////////////////////////////////////");
                }
                else
                {
                    printf("printf(pingIndex: %d     indexReceived:%lu       indexReceived anterior:%lu\n", pingIndex, arrReceivedPingTimestamps[ indexReceived ], arrReceivedPingTimestamps[ indexReceived - 1 ] );
                }
                */
                actualPingTimeWithPrevious = abs( arrReceivedPingTimestamps[ indexReceived ] - arrReceivedPingTimestamps[ indexReceived - 1 ] );
                
                //printf("RESTA PINGS:%"PRIu64"-%"PRIu64"= %"PRIu64"\n",  arrReceivedPingTimestamps[ pingIndex ],arrReceivedPingTimestamps[ indexReceived - 1 ],actualPingTimeWithPrevious); 
                // Actual time between this ping and previous store
                arrPingJitters.push_back( (unsigned long)abs((double)actualPingTimeWithPrevious - (double)timeBetweenPings) );
                #if SHOW_INFO2
                    printf( "PING %d ET: %.3f\n", pingIndex, actualPingTimeWithPrevious );
                #endif
            }
            firstPing= false; 
            indexReceived++; 
        }
        else
        {
            if (calculatePacketLoss)
            {
                packetLossCount++;
            }

            //unsigned long jitterOfPacketLoss = 2 * timeBetweenPings;
            //arrPingJitters.push_back(jitterOfPacketLoss);
            if (!firstPing)
            {
                uint64_t timeOfPacketLoss = arrReceivedPingTimestamps[ indexReceived - 1 ] + timeBetweenPings;
                arrReceivedPingTimestamps.push_back(timeOfPacketLoss);
                indexReceived++; 

            }
            #if SHOW_INFO2
                printf( "PING %d message lost\n", pingIndex);
            #endif
        }
    }
    //printf("%d\n", arrPingJitters.size());
    // Jitter calculation
    jitter = EMathUtils_mean( arrPingJitters );

    if (calculatePacketLoss)
    {
        packetLoss = ((float)(pingMaxCount-indexReceived+packetLossCount)/ (float)(pingsSent)) * 100.f;
    }

    #if SHOW_INFO2
        printf( "Time With previous ping mean: %.3f\n", jitter );
    #endif
}
void Q4SCommonProtocol::calculateJitterStage0(
    Q4SMessageManager &mReceivedMessages, 
    float &jitter, 
    unsigned long timeBetweenPings, 
    unsigned long pingsSent, 
    bool showMeasureInfo)

{
    float packetLoss = 0.f;
    calculateJitter(mReceivedMessages, jitter, timeBetweenPings, pingsSent, false, packetLoss, showMeasureInfo);
}

void Q4SCommonProtocol::calculateJitterAndPacketLossContinuity(
    Q4SMessageManager &mReceivedMessages, 
    float &jitter, 
    unsigned long timeBetweenPings, 
    unsigned long pingsSent, 
    float &packetLoss, 
    bool showMeasureInfo)
{
    calculateJitter(mReceivedMessages, jitter, timeBetweenPings, pingsSent, true, packetLoss, showMeasureInfo);
}

std::set<unsigned long> Q4SCommonProtocol::obtainSortedSequenceNumberList(Q4SMessageManager &mReceivedMessages, std::set<uint64_t> &TimeStamplist, int &BWpacket_size)
{
    std::set<unsigned long> recivedSequenceNumberList;
    uint64_t timeStamp; 
    int packet_size; 
    unsigned long messageSequenceNumber;
    bool flag=true; 
    while (mReceivedMessages.readBandWidthMessage(messageSequenceNumber, true, &timeStamp, &packet_size) && flag)
    {
//printf("TAMAÑO obtainSortedSequenceNumberList:%s\n", packet_size);
        flag= mReceivedMessages.size()!=1; 
        TimeStamplist.insert(timeStamp);
        BWpacket_size=packet_size;
        //printf("time SEQ:%lu, %lu\n", timeStamp, messageSequenceNumber);
        recivedSequenceNumberList.insert(messageSequenceNumber);

    }

    return recivedSequenceNumberList;
}


bool Q4SCommonProtocol::calculateBandwidthPacketLossStage1(Q4SMessageManager &mReceivedMessages, float &packetLoss, uint64_t bandwidthTime, float &bandwidth)
{
    bool ok = false;
    std::set<uint64_t> TimeStamplist;
    int BWpacket_size=0; 
    packetLoss = 0;
    std::set<unsigned long> recivedSequenceNumberList = obtainSortedSequenceNumberList(mReceivedMessages, TimeStamplist, BWpacket_size);
    if (!recivedSequenceNumberList.empty())
    {
        unsigned long packetLossCount = 0;
        unsigned long prevSequenceNumber = 0;
        unsigned long listIndex = 0;
        unsigned long sequenceNumber = 0;        
        uint64_t TimeStampInit;
        uint64_t TimeStampEnd;

        std::set<uint64_t>::iterator itTimeStamp;

        std::set<unsigned long>::iterator it;
        unsigned long totalPacketSent;
        itTimeStamp=TimeStamplist.begin();
        for (it=recivedSequenceNumberList.begin(); it!=recivedSequenceNumberList.end(); ++it)
        {
            if (sequenceNumber==0)
            {
                TimeStampInit= *itTimeStamp;
            }
            //printf("prueba\n");
            if (sequenceNumber != *it)
            {
                packetLossCount += (*it - sequenceNumber);
                sequenceNumber = *it;
                sequenceNumber++;
                //printf("sequenceNumber Perdido: %lu, numero de paquetes perdidos en total: %lu\n", sequenceNumber, packetLossCount );
            }
            else
            {
                sequenceNumber++;
            }
            
            totalPacketSent = *it;
            totalPacketSent++;
            if (itTimeStamp!= TimeStamplist.end())
            {
                TimeStampEnd= *itTimeStamp; 
                itTimeStamp++; 
            }
        }
        //printf("NUmero total de paquetes recibidos%lu\n", sequenceNumber);

        if (totalPacketSent > 0)
        {
            packetLoss = ((float)packetLossCount/ (float)(totalPacketSent)) * 100.f;
            ok = true;
        }
        float ATimestamp = (float)(TimeStampEnd-TimeStampInit);
        
        //printf("timestamp end: %lu\n", TimeStampEnd);
        //printf("timestamp begin: %lu\n", TimeStampInit);
        //printf("DIFERENCIA TIMESTAMP: %f\n", ATimestamp);       

        float numberOfkilobytesPerSecond = (float)sequenceNumber * (float)BWpacket_size / (ATimestamp);//1066
        float kilobitsPerSecond = numberOfkilobytesPerSecond * 8;
        bandwidth = kilobitsPerSecond;
    }
    
    return ok;
}

bool Q4SCommonProtocol::checkStage0(float maxLatency, float maxJitter, Q4SMeasurementResult &results)
{
    bool ok = true;

    if ( results.values.latency > maxLatency )
    {
        results.latencyAlert = true;
        #if SHOW_INFO
            printf( "Lantecy limits not reached: %.3f\n", maxLatency);
        #endif
        ok = false;
    }

    if ( results.values.jitter > maxJitter)
    {
        results.jitterAlert = true;
        #if SHOW_INFO
            printf( "Jitter limits not reached: %.3f\n", maxJitter);
        #endif
        ok = false;
    }

    return ok;
}

bool Q4SCommonProtocol::checkStage0(float maxLatencyUp, float maxJitterUp, float maxLatencyDown, float maxJitterDown, Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
{
    bool ok = true;

    ok &= checkStage0(maxLatencyUp, maxJitterUp, upResults);

    ok &= checkStage0(maxLatencyDown, maxJitterDown, downResults);

    return ok;
}

bool Q4SCommonProtocol::checkStage1(unsigned long bandwidth, float packetLoss, Q4SMeasurementResult &results)
{
    bool ok = true;

    if ( results.values.bandwidth <= (float)bandwidth )
    {
        results.bandwidthAlert = true;
        #if SHOW_INFO
            printf( "BandWidth limits not reached: %d\n", bandwidth);
        #endif
        ok = false;
    }

    if ( results.values.packetLoss > packetLoss)
    {
        results.packetLossAlert = true;
        #if SHOW_INFO
            printf( "PacketLoss limits not reached: %.3f\n", packetLoss);
        #endif
        ok = false;
    }

    return ok;
}

bool Q4SCommonProtocol::checkStage1(unsigned long bandwidthUp, float packetLossUp, unsigned long bandwidthDown, float packetLossDown, Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
{
    bool ok = true;

    ok &= checkStage1(bandwidthUp, packetLossUp, upResults);

    ok &= checkStage1(bandwidthDown, packetLossDown, downResults);

    return ok;
}

bool Q4SCommonProtocol::checkContinuity(float maxLatency, float maxJitter, float maxPacketLoss, Q4SMeasurementResult &results)
{
    bool ok = true;
    
    if ( results.values.latency > maxLatency )
    {
        results.latencyAlert = true;
        #if SHOW_INFO
            printf( "Lantecy limits not reached: %.3f\n", maxLatency);
        #endif
        ok = false;
    }

    if ( results.values.jitter > maxJitter)
    {
        results.jitterAlert = true;
        #if SHOW_INFO
            printf( "Jitter limits not reached: %.3f\n", maxJitter);
        #endif
        ok = false;
    }

    if ( results.values.packetLoss > maxPacketLoss)
    {
        results.packetLossAlert = true;
        #if SHOW_INFO
            printf( "PacketLoss limits not reached: %.3f\n", maxPacketLoss);
        #endif
        ok = false;
    }

    return ok;
}

bool Q4SCommonProtocol::checkContinuity(
    float maxLatencyUp, float maxJitterUp, float maxPacketLossUp, 
    float maxLatencyDown, float maxJitterDown, float maxPacketLossDown, 
    Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
{
    bool ok = true;


    ok &= checkContinuity(maxLatencyUp, maxJitterUp, maxPacketLossUp, upResults);

    ok &= checkContinuity(maxLatencyDown, maxJitterDown, maxPacketLossDown, downResults);

    return ok;
}

void Q4SCommonProtocol::showCheckMessage(Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
{
    #if SHOW_INFO
        if (downResults.latencyAlert)
        {
            printf( "Lantecy Down bad value: %0.3f ms\n", downResults.values.latency);
        }

        if (downResults.jitterAlert)
        {
            printf( "Jitter Down bad value: %0.3f ms\n", downResults.values.jitter);
        }

        if (downResults.bandwidthAlert)
        {
            printf( "BandWidth Down bad value: %0.2f kb/s\n", downResults.values.bandwidth);
        }

        if (downResults.packetLossAlert)
        {
            printf( "PacketLoss Down bad value: %0.3f %\n", downResults.values.packetLoss);
        }

        if (upResults.latencyAlert)
        {
            printf( "Lantecy Up bad value: %0.3f ms\n", upResults.values.latency);
        }

        if (upResults.jitterAlert)
        {
            printf( "Jitter Up bad value: %0.3f ms\n", upResults.values.jitter);
        }

        if (upResults.bandwidthAlert)
        {
            printf( "BandWidth Up bad value: %0.2f kb/s\n", upResults.values.bandwidth);
        }

        if (upResults.packetLossAlert)
        {
            printf( "PacketLoss Up bad value: %0.3f %\n", upResults.values.packetLoss);
        }
    #endif
}
std::string Q4SCommonProtocol::generateAlertMessage(Q4SSDPParams params, Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
{
    //Alert
    std::string alertMessage = "";

    if ( upResults.latencyAlert )
    {
        alertMessage += "LatencyUp: " + std::to_string((long double)upResults.values.latency);
    }

    if ( upResults.jitterAlert )
    {
        alertMessage += " JitterUp: " + std::to_string((long double)upResults.values.jitter);
    }

    if ( upResults.packetLossAlert)
    {
        alertMessage += " PacketLossUp: " + std::to_string((long double)upResults.values.packetLoss);
    }

    if ( downResults.latencyAlert )
    {
        alertMessage += "LatencyDown: " + std::to_string((long double)downResults.values.latency);
    }

    if ( downResults.jitterAlert )
    {
        alertMessage += " JitterDown: " + std::to_string((long double)downResults.values.jitter);
    }

    if ( downResults.packetLossAlert)
    {
        alertMessage += " PacketLossDown: " + std::to_string((long double)downResults.values.packetLoss);
    }

    return alertMessage;
}

std::string Q4SCommonProtocol::generateNotificationAlertMessage(Q4SSDPParams params, Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
{
    //Alert
    std::string alertMessage = "";

    float latency = upResults.values.latency;
    if (downResults.values.latency > upResults.values.latency)
    {
        latency = downResults.values.latency;
    }

    std::ostringstream latencyStream;
    latencyStream << latency;
    std::string latencyString(latencyStream.str());

    alertMessage += "Latency: " + latencyString;

    alertMessage += " ";

    float jitter = upResults.values.jitter;
    if (downResults.values.jitter > upResults.values.jitter)
    {
        jitter = downResults.values.jitter;
    }
    std::ostringstream jitterStream;
    jitterStream << jitter;
    std::string jitterString(jitterStream.str());

    alertMessage += "Jitter: " + jitterString;

    alertMessage += " ";

    float packetLoss = upResults.values.packetLoss;
    if (downResults.values.packetLoss > upResults.values.packetLoss)
    {
        packetLoss = downResults.values.packetLoss;
    }

    std::ostringstream packetLossStream;
    packetLossStream << packetLoss;
    std::string packetLossString(packetLossStream.str());

    alertMessage += "PacketLoss: " + packetLossString;

    return alertMessage;
}

bool Q4SCommonProtocol::LookforCancel(Q4SMessageManager &mReceivedMessages)
{
    bool found= false; 
    Q4SMessageInfo messageInfo;

    found = mReceivedMessages.readCancelMessage();
    return found; 
}