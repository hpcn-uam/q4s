#include "Q4SClientSocket.h"
#include "Q4SClientConfigFile.h"
Q4SClientSocket::Q4SClientSocket ()
{
    clear();
}

Q4SClientSocket::~Q4SClientSocket ()
{
    done();
}

bool Q4SClientSocket::init()
{
    // Prevention done call
    done();

    bool ok = true;
    
    return ok;
}

void Q4SClientSocket::done()
{

}

void Q4SClientSocket::clear()
{
}


bool Q4SClientSocket::openConnection( int socketType )
{
    bool                ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        if( socketType == SOCK_STREAM )
        {
            ok &= connectToServer( &mq4sTcpSocket, socketType );

        }
        else if( socketType == SOCK_DGRAM )
        {
            ok &= connectToServer( &mq4sUdpSocket, socketType );
        }
        else
        {
            ok &= false;
        }
    }

    return ok;
}


bool Q4SClientSocket::closeConnection( int socketType )
{
    bool ok = true;

    if( socketType == SOCK_STREAM )
    {
        printf("closing socket SOCK_STREAM\n");
        ok &= mq4sTcpSocket.shutDown( );
    }
    else if( socketType == SOCK_DGRAM )
    {
        ok &= mq4sUdpSocket.shutDown( );
    }
    else
    {
        ok &= false;
    }

    return ok;
}

bool Q4SClientSocket::sendTcpData( const char* sendBuffer )
{
    return mq4sTcpSocket.sendData( sendBuffer );
}

bool Q4SClientSocket::receiveTcpData( char* receiveBuffer, int receiveBufferSize )
{
    return mq4sTcpSocket.receiveData( receiveBuffer, receiveBufferSize, NULL, q4SClientConfigFile.showSocketReceivedInfo );
}

bool Q4SClientSocket::sendUdpData( const char* sendBuffer )
{
    return mq4sUdpSocket.sendData( sendBuffer );
}

bool Q4SClientSocket::receiveUdpData( char* receiveBuffer, int receiveBufferSize )
{
    return mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize, NULL, q4SClientConfigFile.showSocketReceivedInfo );
}

//////////////////////////////////////////////////////////////////////////////////////////////////



bool Q4SClientSocket::initializeSockets()
{
    int         iResult;
    bool        ok = true;


   
    return ok;
}


bool Q4SClientSocket::connectToServer( Q4SSocket* q4sSocket, int socketType )
{
    //Create a socket.
    struct addrinfo hints,
                    *ptr,
                    *pAddrInfoResult;
    int             iResult;
    bool            ok = true;
    int             socketAttempt = -1;
    int             value= 0; 
    hints.ai_flags = 0;
    hints.ai_family = AF_UNSPEC;
    if( socketType == SOCK_STREAM )
    {
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        // Resolve the local address and port to be used by the server

        iResult = getaddrinfo( q4SClientConfigFile.serverIP.c_str(), q4SClientConfigFile.defaultTCPPort.c_str(), &hints, &pAddrInfoResult );
    }
    else if( socketType == SOCK_DGRAM )
    {
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        // Resolve the local address and port to be used by the server
        iResult = getaddrinfo( q4SClientConfigFile.serverIP.c_str(), q4SClientConfigFile.defaultUDPPort.c_str(), &hints, &pAddrInfoResult );
    }
    else
    {
        ok &= false;
    }

    if( ok && ( iResult != 0 ) )
    {
        printf("%d\n", iResult );
        printf( "getaddrinfo failed\n" );
        printf("Error code: %d\n", errno);
        ok &= false;
    }

    if( ok )
    {
        // Attempt to connect to an address until one succeeds
        for( ptr = pAddrInfoResult; ok && ( ptr != NULL ) && ( socketAttempt <0 ); ptr = ptr->ai_next ) 
        {
            // Create a SOCKET for connecting to server
            socketAttempt = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );

            if( socketAttempt < 0 )
            {
                printf( "socket failed with error\n");
                ok &= false;
            }

            // Connect to server.
            iResult = connect( socketAttempt, ptr->ai_addr, (int)ptr->ai_addrlen );
            if( iResult <0 )
            {
                printf("connect error\n");
                close( socketAttempt );
                socketAttempt = -1;
            }
            if( hints.ai_socktype == SOCK_STREAM )
            {    
                
                setsockopt(socketAttempt, IPPROTO_TCP, TCP_QUICKACK, (char *)&value, sizeof(int));

            } 
            if( hints.ai_socktype == SOCK_DGRAM )
            {    
                int size= 32*1024*1024;
                setsockopt(socketAttempt, SOL_SOCKET, SO_RCVBUF, &size, (socklen_t)sizeof(int));

            }
        }

        freeaddrinfo( pAddrInfoResult );

        if( socketAttempt <= 0 ) 
        {
            printf( "Unable to connect to server!\n" );
            ok &= false;
        }
        else
        {
            q4sSocket->init( );
            if( socketType == SOCK_STREAM )
            {
                q4sSocket->setSocket( socketAttempt, socketType );
            }
            else if( socketType == SOCK_DGRAM )
            {
                q4sSocket->setSocket( socketAttempt, socketType, &q4SClientConfigFile.serverIP, &q4SClientConfigFile.defaultUDPPort );
            }
            else
            {
                ok &= false;
            }
        }
    }

    return ok;
}
