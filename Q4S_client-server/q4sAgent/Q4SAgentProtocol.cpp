#include "Q4SAgentProtocol.h"

#include <stdio.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "../q4sCommon/Q4SMathUtils.h"
#include "Q4SAgentConfigFile.h"
#include "Q4SAgentFunctions.h"


#define     DEFAULT_CONN_ID     1

Q4SAgentProtocol::Q4SAgentProtocol ()
{
    clear();
}

Q4SAgentProtocol::~Q4SAgentProtocol ()
{
    done();
}



bool Q4SAgentProtocol::init()
{
    // Prevention done call
    done();

    bool ok = true;

    if (ok)
    {
        ok &= mAgentSocket.init();
    }

    if (ok)
    {
        ok &= mAgentSocket.startActionSender(); //Envía mensajes al Actuator

    }

    if (ok)
    {
        ok &= mReceivedMessages.init( );
    }

    if (ok)
    {
        ok &= openConnectionListening();
    }
    
    return ok;
}

void Q4SAgentProtocol::done()
{
    closeConnectionListening();
    mReceivedMessages.done( );
}

void Q4SAgentProtocol::clear()
{
}

bool Q4SAgentProtocol::Startlisten(){
	bool ok = true;
	ok=openListen();
	return ok;
}


bool Q4SAgentProtocol::openConnectionListening()
{
    bool ok = true;

    
    //marrthrListenHandle = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
    int thread_error = pthread_create( &marrthrListenHandle, NULL, manageUdpReceivedDataFn, ( void* ) this);

    return ok;
}

bool Q4SAgentProtocol::openListen() // hacer close
{
    bool ok = true;

    //marrthrListenHandle2 = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )listenFn, ( void* ) this, 0, 0 );
    int thread_error = pthread_create( &marrthrListenHandle2, NULL, listenFn, ( void* ) this);

    return ok;
}


void Q4SAgentProtocol::closeConnectionListening()
{
    bool ok = true;

    if( ok )
    {
    	pthread_join(marrthrListenHandle, NULL);
        //WaitForSingleObject( marrthrListenHandle, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}

void Q4SAgentProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        ok &= mAgentSocket.closeConnection( SOCK_DGRAM );
        //WaitForSingleObject( marrthrDataHandle, INFINITE );    	
        pthread_join(marrthrDataHandle, NULL);

    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}

void* Q4SAgentProtocol::listenFn( void* lpData )
{
    Q4SAgentProtocol* q4sCP = ( Q4SAgentProtocol* )lpData;
    return q4sCP->listen( );
}


// State managing functions.

void* Q4SAgentProtocol::listen()
{
    printf("METHOD: listen at port %s\n", q4SAgentConfigFile.listenUDPPort.c_str());
    std::string message;
	 mAgentSocket.sendActionData("SET_BITRATE 5000"); // envía al actuador
    bool ok = true;
    
    if (q4SAgentConfigFile.demoMode)
    {
        mAgentSocket.sendActionData("SET_BITRATE 5000");
        sleep (5000);
        mAgentSocket.sendActionData("SET_BITRATE 4000");
        sleep (5000);
        mAgentSocket.sendActionData("SET_BITRATE 3000");
        sleep (5000);
        mAgentSocket.sendActionData("SET_BITRATE 2000");
        sleep (5000);
        mAgentSocket.sendActionData("SET_BITRATE 1000");
        
        ok = true;
    }
    if ( ok ) 
    {
        mReceivedMessages.readFirst( message ); 
    }
}


void Q4SAgentProtocol::end()
{
    closeConnections();
}

// Received data managing functions.

void* Q4SAgentProtocol::manageUdpReceivedDataFn( void* lpData )
{
    Q4SAgentProtocol* q4sCP = ( Q4SAgentProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}
void* Q4SAgentProtocol::manageUdpReceivedData( )
{
    bool                ok = true;
    char                udpBuffer[ 65536 ];
    unsigned long       bitRate = q4SAgentConfigFile.initialBitRate;
	float				Latency, Jitter;
	unsigned int		Packetloss;
	std::string			udpBufferString, StrLatency, StrJitter, StrPacketloss, TypeAlert, action;
	Actuator			actuator;

    printf("manageUdpReceivedData 1\n");
    mAgentSocket.startUdpListening( );
    printf("manageUdpReceivedData 2\n");

    while ( ok )
    {
        ok &= mAgentSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ));

        if( ok )
        {

			std::stringstream ss(udpBuffer);
			ss >> TypeAlert >> StrLatency >> Latency >> StrJitter >> Jitter;
			printf(" Latency=> %f \n", Latency);
			printf(" Jitter=> %f \n", Jitter);
			printf(" TypeAlert=> %s \n", TypeAlert.c_str());



			// El mensaje de entrada debe ser siempre <ALERT Latency: xxx Jitter: xxxxxxx>
			// ALERT Latency: xxxx
			//	ALERT Jitter: xxxx
            // <ALERT Latency: 121.5 Jitter: 4.89473>
			actuator.ReadConfigFile();
			if (TypeAlert.compare("ALERT") == 0){
				cout << "Rx Alert " << endl;
				actuator.PathAlert(Jitter, Latency, 0, action, TypeAlert);
				printf("Action ALERT=> %s \n", action.c_str());
				if(q4SAgentConfigFile.demoConnSocket){
					mAgentSocket.sendActionData(action.c_str());// ACTUADOR
				}
                else{
					system(action.c_str());
				}
			}
			else if (TypeAlert.compare("RECOVERY") == 0){
				cout << "Rx Recovery " << endl;
				actuator.PathRecovery(action, TypeAlert);
				printf("Action RECOVERY=> %s \n", action.c_str());
				if(q4SAgentConfigFile.demoConnSocket){
					mAgentSocket.sendActionData(action.c_str());//ACTUADOR
				}
				else{
					system(action.c_str());
				}
			}
			else {
				printf("Wrong Type Alert \n");
			}

        }
    }

}