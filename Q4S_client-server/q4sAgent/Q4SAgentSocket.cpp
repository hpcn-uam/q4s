#include "Q4SAgentSocket.h"
#include "Q4SAgentConfigFile.h"
#include <errno.h>

Q4SAgentSocket::Q4SAgentSocket ()
{
    clear();
}

Q4SAgentSocket::~Q4SAgentSocket ()
{
    done();
}

bool Q4SAgentSocket::init()
{
    // Prevention done call
    done();

    bool ok = true;


    return ok;
}

void Q4SAgentSocket::done()
{
}

void Q4SAgentSocket::clear()
{
    mpAddrInfoResultUdp = NULL;
}

bool Q4SAgentSocket::startUdpListening( )
{
    bool                ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        ok &= createUdpSocket( );
    }
    if( ok )
    {
        ok &= bindUdpSocket( );
    }
    if( ok )
    {
        mq4sUdpSocket.setSocket( mUdpSocket, SOCK_DGRAM );
        printf( "Listening UDP at: %s\n", q4SAgentConfigFile.listenUDPPort.c_str() );
    }

    return ok;
}

bool Q4SAgentSocket::closeConnection( int socketType )
{
    bool ok = true;

    if( socketType == SOCK_DGRAM )
    {
        ok &= mq4sUdpSocket.shutDown( );
    }
    else
    {
        ok &= false;
    }

    return ok;
}

/*bool Q4SAgentSocket::sendUdpData( int connectionId, const char* sendBuffer )
{
    bool ok = true;
    if( ok )
    {
        ok &= mq4sUdpSocket.sendData( sendBuffer, &(mpAddrInfoResultUdp) );
    }
    return ok;
}*/

bool Q4SAgentSocket::receiveUdpData( char* receiveBuffer, int receiveBufferSize)
{
    bool                ok = true;
    sockaddr_in         addrInfo;

    if( ok )
    {
        printf("Alert receive 1\n");
        ok &= mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize, &addrInfo, q4SAgentConfigFile.showSocketReceivedInfo );
        printf("Alert receive 2\n");
    }
    
    return ok;
}

bool Q4SAgentSocket::startActionSender()
{
    bool ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        ok &= createActionUdpSocket( );
    }
    if( ok )
    {
        mq4sActionSocket.setSocket( mActionSocket, SOCK_DGRAM, &q4SAgentConfigFile.ganyConnectorIp, &q4SAgentConfigFile.ganyConnectorPort );
        printf( "Prepared for sending commands at: %s\n", q4SAgentConfigFile.ganyConnectorPort.c_str() );
    }

    return ok;
}

bool Q4SAgentSocket::sendActionData( const char* sendBuffer )
{
    bool ok = true;

    if( ok )
    {
        ok &= mq4sActionSocket.sendData( sendBuffer);
    }

    return ok;
}

/////////////////////////////////////////////////////////////////////////////////////

bool Q4SAgentSocket::initializeSockets( )
{
    bool        ok = true;


    return ok;
}

bool Q4SAgentSocket::createUdpSocket( )
{
    //Create a socket.
    struct addrinfo hints;
    int             iResult;
    bool            ok = true;
    
    //ZeroMemory( &hints, sizeof( hints ) );
    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = 0; 
    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo( NULL, q4SAgentConfigFile.listenUDPPort.c_str(), &hints, &mpAddrInfoResultUdp );
    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(iResult));
        ok &= false;
    }
    if( ok )
    {
        mUdpSocket = socket( mpAddrInfoResultUdp->ai_family, mpAddrInfoResultUdp->ai_socktype, mpAddrInfoResultUdp->ai_protocol );
        if( mUdpSocket < 0 ) 
        {
            printf( "Error at socket(): \n" );
            freeaddrinfo( mpAddrInfoResultUdp );
            ok &= false;
        }
    }

    return ok;
}

bool Q4SAgentSocket::bindUdpSocket( )
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;

    if( mUdpSocket < 0 )
    {
        ok &= false;
    }
    else
    {
        sockaddr_in    senderAddr;
        senderAddr.sin_family = AF_INET;
        senderAddr.sin_port = htons( atoi( q4SAgentConfigFile.listenUDPPort.c_str() ) );
        senderAddr.sin_addr.s_addr = htonl( INADDR_ANY ); 
        iResult = bind( mUdpSocket, (struct sockaddr*) &senderAddr, sizeof( senderAddr ) );
        if( iResult < 0 ) 
        {
            printf( "bind failed with error\n");
            freeaddrinfo( mpAddrInfoResultUdp );
            close( mUdpSocket );
            ok &= false;
        }
        else
        {
            freeaddrinfo( mpAddrInfoResultUdp );
        }
    }
    return ok;
}

bool Q4SAgentSocket::createActionUdpSocket( )
{
    //Create a socket.
    struct addrinfo hints;
    int             iResult;
    bool            ok = true;
    
    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = 0; 

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo( NULL, q4SAgentConfigFile.ganyConnectorPort.c_str(), &hints, &mpAddrInfoResultAction );
    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(iResult));
        ok &= false;
    }

    
    if( ok )
    {
        mActionSocket = socket( mpAddrInfoResultAction->ai_family, mpAddrInfoResultAction->ai_socktype, mpAddrInfoResultAction->ai_protocol );
        if( mActionSocket < 0) 
        {
            printf( "Error at socket()\n" );
            freeaddrinfo( mpAddrInfoResultAction );
            ok &= false;
        }
    }

return ok;
}