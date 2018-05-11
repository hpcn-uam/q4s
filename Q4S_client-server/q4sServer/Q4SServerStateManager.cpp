#include "Q4SServerStateManager.h"
#include <stdio.h>
#include <string.h>
#include "Q4SServerConfigFile.h"

Q4SServerStateManager::Q4SServerStateManager()
{
    clear();
}

Q4SServerStateManager::~Q4SServerStateManager()
{
    done();
}


bool Q4SServerStateManager::init()
{
    // Prevention done call
    done();
    
    bool ok = true;

    // Init first state
    ok &= stateInit ( Q4SSERVERSTATE_INIT );

    return ok;
}
void Q4SServerStateManager::done()
{

}
//--private:-------------------------------------------------------------------------------
void Q4SServerStateManager::clear()
{
    q4SServerState = Q4SSERVERSTATE_INVALID;
    nextState =  Q4SSERVERSTATE_INVALID;
    stop = false;
}

bool Q4SServerStateManager::run()
{
    bool ok = true;

    // TODO: set out condition
    
    while (!stop) 
    {
        
        bool stateInitOk = stateInit(nextState);
        if ( stateInitOk )
        {
            q4SServerState = nextState;
        }
    }

    return ok;
}

bool Q4SServerStateManager::stateInit (Q4SServerState state)
{
    stateDone();

    bool ok = true;

    switch (state)
    {
        case Q4SSERVERSTATE_INIT:
            {
                bool initOk = Q4SServerProtocol::init();
                if (initOk)
                {
                    nextState = Q4SSERVERSTATE_HANDSHAKE;
                    printf("Hemos llegado a handshake\n");
                }
                else
                {
                    // TODO: launch error
                    nextState = Q4SSERVERSTATE_TERMINATION;
                }
            }
        break;

        case Q4SSERVERSTATE_HANDSHAKE:
            {
                printf("waiting");
                bool beginOk = Q4SServerProtocol::handshake(mParams);

                if (beginOk)
                {
                    nextState = Q4SSERVERSTATE_NEGOTIATION;
                    printf("Hemos llegado a la negociacion\n");
                }   
                else
                {
                    nextState = Q4SSERVERSTATE_TERMINATION;
                    printf("Hemos llegado a la terminacion 2\n");
                }
                
            }
        break;

        case Q4SSERVERSTATE_NEGOTIATION:
            {
                Q4SMeasurementResult results; 
                bool measureOk = Q4SServerProtocol::negotiation(mParams, results); 
                if (measureOk)
                {
                    nextState = Q4SSERVERSTATE_CONTINUITY;

                } 
                   
                      
                else
                {
                    printf("going to \n");
                    std::string alertMessage;
                    alertMessage= "Latency: " + std::to_string((long double)results.values.latency) + " Jitter: " + std::to_string((long double)results.values.jitter);

                    //Alert
                    Q4SServerProtocol::alert(alertMessage);
                    nextState = Q4SSERVERSTATE_TERMINATION;
                }                              
            }
        break;
        case Q4SSERVERSTATE_CONTINUITY:
            {
                bool readyOk = Q4SServerProtocol::ready(mParams);
                if (readyOk)
                {
                    printf("continuity 1\n");

                    Q4SServerProtocol::continuity(mParams);
                    printf("continuity 2\n");
                }

                std::string alertMessage;
                printf("continuity 3\n");
                alertMessage= "Continuity end";
                Q4SServerProtocol::alert(alertMessage);


                nextState = Q4SSERVERSTATE_TERMINATION;
            }
        
        break;

        case Q4SSERVERSTATE_TERMINATION:
            {
                printf("Hemos llegado a la terminacion\n");
                std::string alertMessage;
                alertMessage= "Termination";
                Q4SServerProtocol::alert(alertMessage);
                //Q4SServerProtocol::end();
                //Q4SServerProtocol::done();
                stop = true;
            }
        break;

        default:
            {
                printf("Error: Wrong state for init");
            }
        break;
    }
    q4SServerState = state;

  	return ok;
}

void Q4SServerStateManager::stateDone ()
{
    switch ( q4SServerState )
    {
        case Q4SSERVERSTATE_INVALID:
        break;

        case Q4SSERVERSTATE_INIT:
        break;

        case Q4SSERVERSTATE_HANDSHAKE:
        break;

        case Q4SSERVERSTATE_NEGOTIATION:
        break;

        case Q4SSERVERSTATE_CONTINUITY:
        break;

        case Q4SSERVERSTATE_TERMINATION:
        break;
    }
}