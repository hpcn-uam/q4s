#ifndef _Q4SAGENTSOCKET_H_
#define _Q4SAGENTSOCKET_H_

#include "../q4sCommon/Q4SSocket.h"

#include <stdio.h>
#include <list>

class Q4SAgentSocket
{
public:

    // Constructor-Destructor
    Q4SAgentSocket( );
    ~Q4SAgentSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    // UDP Listening
    bool    startUdpListening( );
    bool    receiveUdpData (char* receiveBuffer, int receiveBufferSize);

    // Actions
    bool    startActionSender( );
    bool    sendActionData( const char* sendBuffer );

    // Common
    bool    closeConnection( int socketType );

private:

    void    clear( );

    bool    initializeSockets( );
    bool    createUdpSocket( );
    bool    bindUdpSocket( );

    // Action
    bool    createActionUdpSocket( );

    int                 mUdpSocket;
    struct addrinfo*    mpAddrInfoResultUdp; 
    Q4SSocket           mq4sUdpSocket;

    // Action
    int                 mActionSocket;
    struct addrinfo*    mpAddrInfoResultAction; 
    Q4SSocket           mq4sActionSocket;
};

#endif // _Q4SAGENTSOCKET_H_