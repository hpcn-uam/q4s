#include "Q4SServerSocket.h"
#include "Q4SServerConfigFile.h"
#include <errno.h>

Q4SServerSocket::Q4SServerSocket()
{
    clear();
}

Q4SServerSocket::~Q4SServerSocket()
{
    done();
}
void Q4SServerSocket::done()
{
}

bool Q4SServerSocket::startAlertSender()
{
    bool ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        ok &= createAlertUdpSocket( );
    }
    if( ok )
    {
        mq4sAlertSocket.setSocket( mAlertSocket, SOCK_DGRAM, &q4SServerConfigFile.agentIP, &q4SServerConfigFile.agentPort );
        #if SHOW_INFO
        printf( "Prepared for sending alerts at: %s\n", q4SServerConfigFile.agentPort.c_str() );
        #endif 
    }
    return ok;
}

bool Q4SServerSocket::sendAlertData( const char* sendBuffer )
{
    bool ok = true;

    if( ok )
    {
        ok &= mq4sAlertSocket.sendData( sendBuffer);
    }

    return ok;
}
bool Q4SServerSocket::sendTcpData( int connId, const char* sendBuffer )
{
    bool        ok = true;
    Q4SSocket*  pQ4SSocket;

    ok &= getTcpSocket( connId, pQ4SSocket );
    if( ok )
    {
        ok &= q4sTcpSocket->sendData( sendBuffer );
    }

    return ok;
}

bool Q4SServerSocket::receiveTcpData( int connId, char* receiveBuffer, int receiveBufferSize )
{
    bool        ok = true;
    Q4SSocket*  pQ4SSocket;

    ok &= getTcpSocket( connId, pQ4SSocket );
    if( ok )
    {
        ok &= q4sTcpSocket->receiveData( receiveBuffer, receiveBufferSize, NULL, q4SServerConfigFile.showSocketReceivedInfo );
    }

    return ok;
}

bool Q4SServerSocket::startTcpListening( )
{
    bool ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        ok &= createListenSocket( );
    }
    if( ok )
    {
        ok &= bindListenSocket( );
    }
    if( ok )
    {
        ok &= startListen( );
    }    

    return ok;
}
bool Q4SServerSocket::waitForTcpConnection( int connectionId )
{
    bool ok = true;

    if( ok )
    {
        Q4SConnectionInfo* connectionInfo = new Q4SConnectionInfo( );
        connectionInfo->id = connectionId;
        //ZeroMemory( &( connectionInfo->peerTcpAddrInfo ), sizeof( connectionInfo->peerTcpAddrInfo ) );
        //ZeroMemory( &( connectionInfo->peerUdpAddrInfo ), sizeof( connectionInfo->peerUdpAddrInfo ) );
        memset(&( connectionInfo->peerTcpAddrInfo ),0, sizeof( connectionInfo->peerTcpAddrInfo ));

        ok &= acceptClientConnection( connectionInfo );
    }

    return ok;
}

bool Q4SServerSocket::stopWaiting( )
{
    bool ok = true;

    if( ok )
    {
        //ok &= closeListenSocket( );        
        close(mAlertSocket);
        close(mUdpSocket);
        close(mListenSocket);


    }

    return ok;
}

bool Q4SServerSocket::closeConnection( int socketType )
{
    bool ok = true;

    if( socketType == SOCK_DGRAM )
    {
        ok &= mq4sUdpSocket.shutDown( );
    }
    else if( socketType == SOCK_STREAM )
    {
        listConnectionInfo.front( )->q4sTcpSocket.shutDown( );
        q4sTcpSocket->shutDown( ); 
    }
    else
    {
        ok &= false;
    }

    return ok;
}


bool Q4SServerSocket::sendUdpData( int connectionId, const char* sendBuffer )
{
    bool                ok = true;
    Q4SConnectionInfo*  pQ4SConnInfo;

    ok &= getConnectionInfo( connectionId, pQ4SConnInfo );
    if( ok )
    {
        ok &= mq4sUdpSocket.sendData( sendBuffer, &( pQ4SConnInfo->peerUdpAddrInfo ) );
    }

    return ok;
}

bool Q4SServerSocket::sendUdpBWData( int connectionId, const char* sendBuffer )
{
    bool                ok = true;
    Q4SConnectionInfo*  pQ4SConnInfo;

    ok &= getConnectionInfo( connectionId, pQ4SConnInfo );
    if( ok )
    {
        ok &= mq4sUdpSocket.sendBWData( sendBuffer, &( pQ4SConnInfo->peerUdpAddrInfo ) );
    }

    return ok;
}

bool Q4SServerSocket::startUdpListening( )
{
    bool ok = true;

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
        #if SHOW_INFO
        printf( "Listening UDP at: %s\n", q4SServerConfigFile.defaultUDPPort.c_str() );
        #endif
    }

    return ok;
}

bool Q4SServerSocket::receiveUdpData( char* receiveBuffer, int receiveBufferSize, int& connectionId )
{
    bool                ok = true;
    Q4SConnectionInfo*  pQ4SConnInfo;
    sockaddr_in         addrInfo;

    if( ok )
    {
        //printf("Receive Udp Data\n");
        ok &= mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize, &addrInfo, q4SServerConfigFile.showSocketReceivedInfo );
    }
    if( ok )
    {
        ok &= getConnectionInfo( addrInfo, pQ4SConnInfo );
    }
    if( ok )
    {
        //printf("connect\n");
        connectionId = pQ4SConnInfo->id;
        memcpy( &( pQ4SConnInfo->peerUdpAddrInfo ), &addrInfo, sizeof( addrInfo ) );
    }
    
    return ok;
}


/////////////////////////////////////////////////////////////////////////////////////



bool Q4SServerSocket::initializeSockets()
{
    //WSADATA     wsaData;
    bool        ok = true;


   
    return ok;
}

/*
bool Q4SServerSocket::createAlertUdpSocket()
{
    int     iResult; 
    bool    ok=true;
    mAlertSocket = socket(AF_INET, SOCK_DGRAM,0); // ESE 0 HAY QUE REVISARLO
    if (mAlertSocket== 0)
    {
        printf("Error at socket");
    }
    return ok; 
}
*/
bool Q4SServerSocket::createAlertUdpSocket( )
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
    iResult = getaddrinfo( q4SServerConfigFile.agentIP.c_str(), q4SServerConfigFile.agentPort.c_str(), &hints, &mpAddrInfoResultAlert );
    //iResult = getaddrinfo( NULL, q4SServerConfigFile.agentPort.c_str(), &hints, &mpAddrInfoResultAlert );
        
    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(iResult));
        ok &= false;
    }

    if( ok )
    {
        mAlertSocket = socket( mpAddrInfoResultAlert->ai_family, mpAddrInfoResultAlert->ai_socktype, mpAddrInfoResultAlert->ai_protocol );
        if( mAlertSocket <0 ) 
        {
            printf( "Error at socket()\n" );
            printf("Error code: %d\n", errno);
            freeaddrinfo( mpAddrInfoResultAlert );
            ok &= false;
        }
    }
    if( ok )
    {
        // Connect to server.
        iResult = connect( mAlertSocket, mpAddrInfoResultAlert->ai_addr, (int)mpAddrInfoResultAlert->ai_addrlen );
        if( iResult <0 )
        {
            printf("connect error\n");
            close( mAlertSocket );
            mAlertSocket = -1;
        }
    }

    return ok;
}
void Q4SServerSocket::clear()
{
    mListenSocket   = -1;
    mUdpSocket      = -1;
    mAlertSocket    = -1;
    mpAddrInfoResultTcp     = NULL;
    mpAddrInfoResultUdp     = NULL;
    mpAddrInfoResultAlert   = NULL;
}

bool Q4SServerSocket::getTcpSocket( int connectionId, Q4SSocket*& pQ4SSocket )
{
    bool ok = false;

    std::list< Q4SConnectionInfo* >::iterator   itr_conn;
    pQ4SSocket = NULL;

    for( itr_conn = listConnectionInfo.begin( ); ( pQ4SSocket == NULL ) && ( itr_conn != listConnectionInfo.end( ) ); itr_conn++ )
    {
        if( ( *itr_conn )->id == connectionId )
        {
            pQ4SSocket = &( ( *itr_conn )->q4sTcpSocket );
            ok = true;
        }
    }

    return ok;
}
bool Q4SServerSocket::createListenSocket( )
{
    //Create a socket.
    struct addrinfo hints;
    int             iResult;
    bool            ok = true;
    //hints=(struct addrinfo*)calloc( 1,sizeof( struct addrinfo ));
    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo( NULL, q4SServerConfigFile.defaultTCPPort.c_str(), &hints, &mpAddrInfoResultTcp );

    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        //WSACleanup( );
        ok &= false;
    }

    if( ok )
    {
        // Create a SOCKET for the server to listen for client connections
        mListenSocket = socket( mpAddrInfoResultTcp->ai_family, mpAddrInfoResultTcp->ai_socktype, mpAddrInfoResultTcp->ai_protocol );
        int optval= 1; 
        setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
        if( mListenSocket <=0 ) 
        {
            //printf( "Error at socket(): %ld\n", WSAGetLastError( ) );
            freeaddrinfo( mpAddrInfoResultTcp );
            //WSACleanup( );
            ok &= false;
        }
    }

    return ok;
}



bool Q4SServerSocket::bindListenSocket( )
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;
    int value = 0; 
    // Setup the TCP listening socket
    if( mListenSocket <= 0)
    {
        ok &= false;
    }
    else
    {
        iResult = bind( mListenSocket, mpAddrInfoResultTcp->ai_addr, (int)mpAddrInfoResultTcp->ai_addrlen );
         setsockopt(mListenSocket, IPPROTO_TCP, TCP_QUICKACK, (char *)&value, sizeof(int));
    }

    if( ok )
    {

        if( iResult == -1 ) 
        {
            printf( "bind failed with error\n");
            printf("Error code: %d\n", errno);           
            freeaddrinfo( mpAddrInfoResultTcp );
            close( mListenSocket );
            //WSACleanup( );
            ok &= false;
        }
        else
        {
            freeaddrinfo( mpAddrInfoResultTcp );
        }
    }

    return ok;
}

bool Q4SServerSocket::startListen( )
{
    //Listen on the socket for a client.
    bool ok = true;

    if( listen( mListenSocket, SOMAXCONN ) < 0)
    {
        printf( "Listen failed with error\n");
        close( mListenSocket );
        //WSACleanup( );
        ok &= false;
    }

    //printf( "Listening TCP at: %s\n", q4SServerConfigFile.defaultTCPPort.c_str() );

    return ok;
}


bool Q4SServerSocket::acceptClientConnection( Q4SConnectionInfo* connectionInfo )
{
    //Accept a connection from a client.
    int  attemptSocket = -1;
    bool    ok = true;
    socklen_t     addrlen;
    int value = 0; 
    addrlen = sizeof( connectionInfo->peerTcpAddrInfo );
    // Accept a client socket.
    attemptSocket = accept( mListenSocket, ( sockaddr* )&( connectionInfo->peerTcpAddrInfo ), &addrlen );
    setsockopt(mListenSocket, IPPROTO_TCP, TCP_QUICKACK, (char *)&value, sizeof(int));

    if( attemptSocket <=0 )
    {   
        printf( "accept interrupted\n" );
        
        close( attemptSocket );
        //WSACleanup( );
        ok &= false;
    }
    else
    {

        connectionInfo->q4sTcpSocket.init( );
        connectionInfo->q4sTcpSocket.setSocket( attemptSocket, SOCK_STREAM );
        listConnectionInfo.push_back( connectionInfo );
        q4sTcpSocket= &connectionInfo->q4sTcpSocket; 
    }

    return ok;
}

bool Q4SServerSocket::closeListenSocket( )
{
    bool ok = true;

    close( mListenSocket );

    return ok;
}

bool Q4SServerSocket::getConnectionInfo( int connectionId, Q4SConnectionInfo*& pQ4SConnInfo )
{
    bool ok = false;

    std::list< Q4SConnectionInfo* >::iterator   itr_conn;
    pQ4SConnInfo = NULL;

    for( itr_conn = listConnectionInfo.begin( ); ( pQ4SConnInfo == NULL ) && ( itr_conn != listConnectionInfo.end( ) ); itr_conn++ )
    {
        if( ( *itr_conn )->id == connectionId )
        {
            pQ4SConnInfo = ( *itr_conn );
            ok = true;
        }
    }

    return ok;
}

bool Q4SServerSocket::getConnectionInfo( sockaddr_in& connectionInfo, Q4SConnectionInfo*& pQ4SConnInfo )
{
    bool ok = false;

    std::list< Q4SConnectionInfo* >::iterator   itr_conn;
    Q4SConnectionInfo*                          pQ4SConnInfoFromTcp;

    pQ4SConnInfo = NULL;
    pQ4SConnInfoFromTcp = NULL;

    for( itr_conn = listConnectionInfo.begin( ); ( pQ4SConnInfo == NULL ) && ( itr_conn != listConnectionInfo.end( ) ); itr_conn++ )
    {
        if( ( ( *itr_conn )->peerUdpAddrInfo.sin_addr.s_addr != 0 ) &&
            ( ( *itr_conn )->peerUdpAddrInfo.sin_addr.s_addr == connectionInfo.sin_addr.s_addr ) &&
            ( ( *itr_conn )->peerUdpAddrInfo.sin_port == connectionInfo.sin_port ) )
        {
            // First we try to get connection according to udp peer address information.
            pQ4SConnInfo = ( *itr_conn );
            ok = true;
        }
        else
        {
            // There's no udp address information. Perhaps it can be a new tcp connection that fits.
            if( ( pQ4SConnInfoFromTcp == NULL ) &&
                ( ( *itr_conn )->peerTcpAddrInfo.sin_addr.s_addr == connectionInfo.sin_addr.s_addr ) )
            {
                pQ4SConnInfoFromTcp = ( *itr_conn );
            }
        }
    }

    if( ( pQ4SConnInfo == NULL ) &&
        ( pQ4SConnInfoFromTcp != NULL) )
    {
        pQ4SConnInfo = pQ4SConnInfoFromTcp;
        ok = true;
    }

    return ok;
}

bool Q4SServerSocket::createUdpSocket( )
{
    //Create a socket.
    struct addrinfo hints;
    int             iResult;
    bool            ok = true;
    
    //ZeroMemory( &hints, sizeof( hints ) );
    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo( NULL, q4SServerConfigFile.defaultUDPPort.c_str(), &hints, &mpAddrInfoResultUdp );
        
    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        //WSACleanup( );
        ok &= false;
    }

    if( ok )
    {
        int size= 232*1024*1024;
        mUdpSocket = socket( mpAddrInfoResultUdp->ai_family, mpAddrInfoResultUdp->ai_socktype, mpAddrInfoResultUdp->ai_protocol );
        if( mUdpSocket <=0 ) 
        {
            printf( "Error at socket(): %d\n", mUdpSocket);
            freeaddrinfo( mpAddrInfoResultUdp );
            //WSACleanup( );
            ok &= false;
        }
        setsockopt(mUdpSocket, SOL_SOCKET, SO_RCVBUF, &size, (socklen_t)sizeof(int));
    }

    return ok;
}

bool Q4SServerSocket::bindUdpSocket( )
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;
    //printf("\n \nbindUdpSocket \n \n");
    if( mUdpSocket <= 0 )
    {
        ok &= false;
    }
    else
    {
        sockaddr_in    senderAddr;
        senderAddr.sin_family = AF_INET;
        senderAddr.sin_port = htons( atoi( q4SServerConfigFile.defaultUDPPort.c_str() ) );
        //printf("%d\n",atoi( q4SServerConfigFile.defaultUDPPort.c_str() ));
        senderAddr.sin_addr.s_addr = htonl( INADDR_ANY ); 
        iResult = bind( mUdpSocket, ( struct sockaddr* ) &senderAddr, sizeof( senderAddr ) );
    }

    if( ok )
    {
        if( iResult <0 ) 
        {                  
            printf( "bind udp failed with error\n" );
            printf("Error code: %d\n", errno);
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
