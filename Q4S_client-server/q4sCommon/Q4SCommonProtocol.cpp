#include "Q4SCommonProtocol.h"

#include <stdio.h>
#include "Q4SMessageInfo.h"
#include "Q4SMessageManager.h"
#include "Q4SMathUtils.h"
#include <sstream>



void Q4SCommonProtocol::calculateLatency(Q4SMessageManager &mReceivedMessages, std::vector<unsigned long> &arrSentPingTimestamps, float &latency, unsigned long pingsSent, bool showMeasureInfo)
{
    Q4SMessageInfo messageInfo;
    std::vector<float> arrPingLatencies;
    float actualPingLatency;
    int pingIndex = 0;
    int pingMaxCount = pingsSent;

    // Prepare for Latency calculation
    for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
    {
        if( mReceivedMessages.read200OKMessage( messageInfo, true ) == true )
        {
            // Actual ping latency calculation
            actualPingLatency = (messageInfo.timeStamp - arrSentPingTimestamps[ pingIndex ])/ 2.0f;
            //printf("TIEMPO 200 0k: %lu\nTIEMPO PING: %lu\n",messageInfo.timeStamp, arrSentPingTimestamps[ pingIndex ]);
            // Latency store
            arrPingLatencies.push_back( actualPingLatency );

            if (showMeasureInfo)
            {
                printf( "PING %d actual ping latency: %.3f\n", pingIndex, actualPingLatency );
            }
        }
        else
        {
            if (showMeasureInfo)
            {
                printf( "PING RESPONSE %d message lost\n", pingIndex);
            }
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
    std::vector<unsigned long> arrReceivedPingTimestamps;
    std::vector<unsigned long> arrPingJitters;
    unsigned long actualPingTimeWithPrevious;
    unsigned long packetLossCount = 0;

    // Prepare for Jitter calculation
    for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
    {
        if( mReceivedMessages.readPingMessage( pingIndex, messageInfo, true ) == true )
        {
            arrReceivedPingTimestamps.push_back( messageInfo.timeStamp );

            if( pingIndex > 0 )
            {
                // Actual time between this ping and previous calculation
                actualPingTimeWithPrevious = ( arrReceivedPingTimestamps[ pingIndex ] - arrReceivedPingTimestamps[ pingIndex - 1 ] );

                // Actual time between this ping and previous store
                arrPingJitters.push_back( (unsigned long)abs((double)actualPingTimeWithPrevious - (double)timeBetweenPings) );

                if (showMeasureInfo)
                {
                    printf( "PING %d ET: %.3f\n", pingIndex, actualPingTimeWithPrevious );
                }
            }
        }
        else
        {
            if (calculatePacketLoss)
            {
                packetLossCount++;
            }

            unsigned long jitterOfPacketLoss = 2 * timeBetweenPings;
            arrPingJitters.push_back(jitterOfPacketLoss);

            unsigned long timeOfPacketLoss = arrReceivedPingTimestamps[ pingIndex - 1 ] + timeBetweenPings;
            arrReceivedPingTimestamps.push_back(timeOfPacketLoss);
            if (showMeasureInfo)
            {
                printf( "PING %d message lost\n", pingIndex);
            }
        }
    }

    // Jitter calculation
    jitter = EMathUtils_mean( arrPingJitters );

    if (calculatePacketLoss)
    {
        packetLoss = ((float)packetLossCount/ (float)(pingsSent)) * 100.f;
    }

    if (showMeasureInfo)
    {
        printf( "Time With previous ping mean: %.3f\n", jitter );
    }
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

std::set<unsigned long> Q4SCommonProtocol::obtainSortedSequenceNumberList(Q4SMessageManager &mReceivedMessages)
{
    std::set<unsigned long> recivedSequenceNumberList;

    unsigned long messageSequenceNumber;

    while (mReceivedMessages.readBandWidthMessage(messageSequenceNumber, true) )
    {
        recivedSequenceNumberList.insert(messageSequenceNumber);
    }

    return recivedSequenceNumberList;
}


bool Q4SCommonProtocol::calculateBandwidthPacketLossStage1(Q4SMessageManager &mReceivedMessages, float &packetLoss, unsigned long bandwidthTime, float &bandwidth)
{
    bool ok = false;

    packetLoss = 0;
    printf("%d\n", sizeof(mReceivedMessages));
    std::set<unsigned long> recivedSequenceNumberList = obtainSortedSequenceNumberList(mReceivedMessages);

    if (!recivedSequenceNumberList.empty())
    {
        unsigned long packetLossCount = 0;
        unsigned long prevSequenceNumber = 0;
        unsigned long listIndex = 0;
        unsigned long sequenceNumber = 0;
        std::set<unsigned long>::iterator it;
        unsigned long totalPacketSent;

        for (it=recivedSequenceNumberList.begin(); it!=recivedSequenceNumberList.end(); ++it)
        {
            if (sequenceNumber != *it)
            {
                packetLossCount += (*it - sequenceNumber);
                sequenceNumber = *it;
                sequenceNumber++;
                printf("%d, %d\n", sequenceNumber, packetLossCount);
            }
            else
            {
                sequenceNumber++;
            }

            totalPacketSent = *it;
            totalPacketSent++;
        }


        if (totalPacketSent > 0)
        {
            packetLoss = ((float)packetLossCount/ (float)(totalPacketSent)) * 100.f;
            ok = true;
        }
        float numberOfPacketsPerSecond = (float)sequenceNumber * 1066.f / (float)bandwidthTime;//1066
        float kilobitsPerSecond = numberOfPacketsPerSecond * 8;
        bandwidth = kilobitsPerSecond;
    }
    
    return ok;
}

bool Q4SCommonProtocol::checkStage0(unsigned long maxLatency, unsigned long maxJitter, Q4SMeasurementResult &results)
{
    bool ok = true;

    if ( results.values.latency > (float)maxLatency )
    {
        results.latencyAlert = true;
        printf( "Lantecy limits not reached: %d\n", maxLatency);
        ok = false;
    }

    if ( results.values.jitter > (float)maxJitter)
    {
        results.jitterAlert = true;
        printf( "Jitter limits not reached: %d\n", maxJitter);
        ok = false;
    }

    return ok;
}

bool Q4SCommonProtocol::checkStage0(unsigned long maxLatencyUp, unsigned long maxJitterUp, unsigned long maxLatencyDown, unsigned long maxJitterDown, Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
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
        printf( "BandWidth limits not reached: %d\n", bandwidth);
        ok = false;
    }

    if ( results.values.packetLoss > packetLoss)
    {
        results.packetLossAlert = true;
        printf( "PacketLoss limits not reached: %.3f\n", packetLoss);
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

bool Q4SCommonProtocol::checkContinuity(unsigned long maxLatency, unsigned long maxJitter, float maxPacketLoss, Q4SMeasurementResult &results)
{
    bool ok = true;
    
    if ( results.values.latency > (float)maxLatency )
    {
        results.latencyAlert = true;
        printf( "Lantecy limits not reached: %d\n", maxLatency);
        ok = false;
    }

    if ( results.values.jitter > (float)maxJitter)
    {
        results.jitterAlert = true;
        printf( "Jitter limits not reached: %d\n", maxJitter);
        ok = false;
    }

    if ( results.values.packetLoss > maxPacketLoss)
    {
        results.packetLossAlert = true;
        printf( "PacketLoss limits not reached: %.3f\n", maxPacketLoss);
        ok = false;
    }

    return ok;
}

bool Q4SCommonProtocol::checkContinuity(
    unsigned long maxLatencyUp, unsigned long maxJitterUp, float maxPacketLossUp, 
    unsigned long maxLatencyDown, unsigned long maxJitterDown, float maxPacketLossDown, 
    Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
{
    bool ok = true;


    ok &= checkContinuity(maxLatencyUp, maxJitterUp, maxPacketLossUp, upResults);

    ok &= checkContinuity(maxLatencyDown, maxJitterDown, maxPacketLossDown, downResults);

    return ok;
}

void Q4SCommonProtocol::showCheckMessage(Q4SMeasurementResult &upResults, Q4SMeasurementResult &downResults)
{
    if (downResults.latencyAlert)
    {
        printf( "Lantecy Down bad value: %0.3f\n", downResults.values.latency);
    }

    if (downResults.jitterAlert)
    {
        printf( "Jitter Down bad value: %0.3f\n", downResults.values.jitter);
    }

    if (downResults.bandwidthAlert)
    {
        printf( "BandWidth Down bad value: %0.2f\n", downResults.values.bandwidth);
    }

    if (downResults.packetLossAlert)
    {
        printf( "PacketLoss Down bad value: %0.3f\n", downResults.values.packetLoss);
    }

    if (upResults.latencyAlert)
    {
        printf( "Lantecy Up bad value: %0.3f\n", upResults.values.latency);
    }

    if (upResults.jitterAlert)
    {
        printf( "Jitter Up bad value: %0.3f\n", upResults.values.jitter);
    }

    if (upResults.bandwidthAlert)
    {
        printf( "BandWidth Up bad value: %0.2f\n", upResults.values.bandwidth);
    }

    if (upResults.packetLossAlert)
    {
        printf( "PacketLoss Up bad value: %0.3f\n", upResults.values.packetLoss);
    }
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