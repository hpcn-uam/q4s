
#include "Q4SClientStateManager.h"

#include <stdio.h>

#include "Q4SClientConfigFile.h"

Q4SClientStateManager::Q4SClientStateManager()
{
    clear();
}

Q4SClientStateManager::~Q4SClientStateManager()
{
    done();
}

bool Q4SClientStateManager::init()
{
    // Prevention done call
    done();
    
    bool ok = true;

    // Init first state
    ok &= stateInit ( Q4SCLIENTSTATE_INIT );

    return ok;
}

void Q4SClientStateManager::done()
{

}



void Q4SClientStateManager::clear()
{
    q4SClientState = Q4SCLIENTSTATE_INVALID;
    nextState =  Q4SCLIENTSTATE_INVALID;
    stop = false;
}

bool Q4SClientStateManager::run()
{
    bool ok = true;

    // TODO: set out condition
    while (!stop) 
    {
        bool stateInitOk = stateInit(nextState);
        if ( stateInitOk )
        {
            q4SClientState = nextState;
        }
    }

    return ok;
}


bool Q4SClientStateManager::stateInit (Q4SClientState state)
{
    stateDone();

    bool ok = true;

    switch (state)
    {
        case Q4SCLIENTSTATE_INIT:
            {
                // Initialize protocol: thread and sockets
                bool initOk = Q4SClientProtocol::init(q4SClientConfigFile.serverConnectionRetryTimes, q4SClientConfigFile.serverConnectionMilisecondsBetweenTimes);
                if (initOk)
                {
                    nextState = Q4SCLIENTSTATE_HANDSHAKE;
                }
                else
                {
                    // TODO: launch error
                    nextState = Q4SCLIENTSTATE_TERMINATION;
                }
            }
        break;

        case Q4SCLIENTSTATE_HANDSHAKE:
            {
                // Trigger the establishment of the protocol
                bool beginOk = Q4SClientProtocol::handshake(mParams);                
                if (beginOk)
                {
                    printf( "Limits - Latency: %d\n", mParams.latency);
                    printf( "Limits - Jitter Up: %d\n", mParams.jitterUp);
                    printf( "Limits - Jitter Down: %d\n", mParams.jitterDown);
                    printf( "Limits - Bandwidth Up: %d\n", mParams.bandWidthUp);
                    printf( "Limits - Bandwidth Down: %d\n", mParams.bandWidthDown);
                    printf( "Limits - PacketLoss Up: %.3f\n", mParams.packetLossUp);
                    printf( "Limits - PacketLoss Down: %.3f\n", mParams.packetLossDown);
                    printf( "Params - QOSLevelDown: %d\n", mParams.qosLevelDown);
                    printf( "Params - QOSLevelUp: %d\n", mParams.qosLevelUp);
                    printf( "Params - AlertPause: %lu\n", mParams.alertPause);
                    printf( "Params - RecoveryPause: %lu\n", mParams.recoveryPause);
                    printf( "Params - NegotiationTimeBetweenPings Up: %lu\n", mParams.procedure.negotiationTimeBetweenPingsUplink);
                    printf( "Params - NegotiationTimeBetweenPings Down: %lu\n", mParams.procedure.negotiationTimeBetweenPingsDownlink);
                    printf( "Params - ContinuityTimeBetweenPings Up: %lu\n", mParams.procedure.continuityTimeBetweenPingsUplink);
                    printf( "Params - ContinuityTimeBetweenPings Down: %lu\n", mParams.procedure.continuityTimeBetweenPingsDownlink);
                    printf( "Params - BandWidthTime: %lu\n", mParams.procedure.bandwidthTime);
                    printf( "Params - WindowSizeLatencyCalc Up: %lu\n", mParams.procedure.windowSizeLatencyCalcUplink);
                    printf( "Params - WindowSizeLatencyCalc Down: %lu\n", mParams.procedure.windowSizeLatencyCalcDownlink);
                    printf( "Params - WindowSizePacketLossCalc Up: %lu\n", mParams.procedure.windowSizePacketLossCalcUplink);
                    printf( "Params - WindowSizePacketLossCalc Down: %lu\n", mParams.procedure.windowSizePacketLossCalcDownlink);
                    
                    nextState = Q4SCLIENTSTATE_NEGOTIATION;
                }
                else
                {
                    // TODO: launch error
                    nextState = Q4SCLIENTSTATE_TERMINATION;
                }
            }
        break;

        case Q4SCLIENTSTATE_NEGOTIATION:
            {
                Q4SMeasurementResult results;
                bool measureOk = Q4SClientProtocol::negotiation(mParams, results);
                if( measureOk )
                {
                    if (q4SClientConfigFile.waitForLaunchGANY)
                    {
                        waitForLaunchGANY();
                    }
                    nextState = Q4SCLIENTSTATE_CONTINUITY;
                }
                else
                {
                    
                    nextState = Q4SCLIENTSTATE_TERMINATION;
                }
            } 
        break;

        case Q4SCLIENTSTATE_CONTINUITY:
            {
               bool readyOk = Q4SClientProtocol::ready(0, mParams);
                if( readyOk )
                {                    
                    continuity(mParams);
                    nextState = Q4SCLIENTSTATE_TERMINATION;
                }
                else
                {
                    // TODO: launch error
                    nextState = Q4SCLIENTSTATE_TERMINATION;
                } 
            }
        break;

        case Q4SCLIENTSTATE_TERMINATION:
            {
                printf("Termination state\n");
                //Q4SClientProtocol::done( );
                stop = true;
            }
        break;

        default:
            {
                printf("Error: Wrong state for init");
            }
        break;
  }

  q4SClientState = state;

  return ok;
}

void Q4SClientStateManager::stateDone ()
{
    switch ( q4SClientState )
    {
        case Q4SCLIENTSTATE_INVALID:
        break;

        case Q4SCLIENTSTATE_INIT:
        break;

        case Q4SCLIENTSTATE_HANDSHAKE:
        break;

        case Q4SCLIENTSTATE_NEGOTIATION:
        break;

        case Q4SCLIENTSTATE_CONTINUITY:
        break;

        case Q4SCLIENTSTATE_TERMINATION:
        break;
    }
}

void Q4SClientStateManager::waitForLaunchGANY()
 {
     printf("Negotiation OK, launch GANY and press any key\n");
     while(!getchar())
     {
     }
}