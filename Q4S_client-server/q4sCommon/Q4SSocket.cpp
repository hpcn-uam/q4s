#include "Q4SSocket.h"
#include <errno.h>


Q4SSocket::Q4SSocket( )
{
    clear();
}

Q4SSocket::~Q4SSocket( )
{
    done();
}

bool Q4SSocket::init( )
{
    // Prevention done call
    done();

    bool ok = true;
    return ok;
}

void Q4SSocket::done( )
{
}

void Q4SSocket::clear( )
{
    mSocket = 0;
    mSocketType = 0;
    mPeerAddrInfoLen = 0;
    mAlreadyReceived = false;
}


void Q4SSocket::setSocket( int socket, int socketType, std::string* connectToIP, std::string* connectToUDPPort)
{
    mSocket = socket;
    mSocketType = socketType;
    mSocketUDPConnectToIP = connectToIP;
    mSocketUDPConnectToPort = connectToUDPPort;
}

bool Q4SSocket::sendData( const char* sendBuffer, sockaddr_in* pAddrInfo, bool showInfo)
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;
    // Send the buffer
    if( mSocketType == SOCK_STREAM )
    {
        iResult = send( mSocket, sendBuffer, (int)strlen( sendBuffer ), 0 );
    }
    else if( mSocketType == SOCK_DGRAM )
    {
        sockaddr*   addrInfoToUse;
        int         addrInfoLenToUse;
        // In client connections, we send prior to receive, we haven't peer info. We define it.
        if( pAddrInfo != NULL )
        {
            addrInfoToUse = ( sockaddr* )pAddrInfo;
            addrInfoLenToUse = sizeof( *pAddrInfo );
        }
        else 
        {
            if( mAlreadyReceived == false )
            {
                mPeerAddrInfo.sin_family = AF_INET;
                mPeerAddrInfo.sin_port = htons( atoi( mSocketUDPConnectToPort->c_str() ) );
                mPeerAddrInfo.sin_addr.s_addr = inet_addr( mSocketUDPConnectToIP->c_str() );
                mPeerAddrInfoLen = sizeof( mPeerAddrInfo );
            }
            addrInfoToUse = ( sockaddr* )&mPeerAddrInfo;
            addrInfoLenToUse = mPeerAddrInfoLen;
        }
        iResult = sendto( mSocket, sendBuffer, (int)strlen( sendBuffer ), 0, addrInfoToUse, addrInfoLenToUse );

    }
    else
    {
        ok &= false;
    }

    if( iResult <= 0 )
    {
        printf( "send failed with error\n" );
        disconnect( );
        ok &= false;
    }

    if ( showInfo) 
    {
        printf( "Bytes Sent: %d\n", iResult );
    }

    return ok;
}

bool Q4SSocket::sendBWData( const char* sendBuffer, sockaddr_in* pAddrInfo)
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;

    // Send the buffer
    sockaddr*   addrInfoToUse;
    int         addrInfoLenToUse;
    // In client connections, we send prior to receive, we haven't peer info. We define it.
    if( pAddrInfo != NULL )
    {
        addrInfoToUse = ( sockaddr* )pAddrInfo;
        addrInfoLenToUse = sizeof( *pAddrInfo );
    }
    else
    {
        addrInfoToUse = ( sockaddr* )&mPeerAddrInfo;
        addrInfoLenToUse = mPeerAddrInfoLen;
    }
    iResult = sendto( mSocket, sendBuffer, (int)strlen( sendBuffer ), 0, addrInfoToUse, addrInfoLenToUse );
 
    if( iResult <= 0 )
    {
        printf( "send failed with error\n" );
        disconnect( );
        ok &= false;
    }

    return ok;
}


bool Q4SSocket::receiveData( char* receiveBuffer, int receiveBufferSize, sockaddr_in* pAddrInfo, bool showInfo)
{
    //Listen on the socket for a client.
    bool    ok = true;
    int     iResult;
    int value= 0; 

/*
    if( mSocketType == SOCK_STREAM )
    {
    }
    else if( mSocketType == SOCK_DGRAM )
    {
    }
    else
    {
        ok &= false;
    }
    */
    if( mPeerAddrInfoLen == 0 )
    {
        mPeerAddrInfoLen = (socklen_t)sizeof( mPeerAddrInfo );
    }


    iResult = recvfrom( mSocket, receiveBuffer, receiveBufferSize, 0, ( sockaddr* )&mPeerAddrInfo, &mPeerAddrInfoLen );
    if (iResult < 0)
    {
        printf( "Error at receiveData \n" );
        printf("Error code: %d\n", errno); 
    }
    if( mSocketType == SOCK_STREAM )
    {
        setsockopt(mSocket, IPPROTO_TCP, TCP_QUICKACK, (char *)&value, sizeof(int));
    } 

    if( pAddrInfo != NULL )
    {
        memcpy( pAddrInfo, &mPeerAddrInfo, sizeof( mPeerAddrInfo ) );
    }
/*
    if ( showInfo) 
    {
        printf( "Exiting from receive in %d type\n", mSocketType );
    }
*/
    if( iResult > 0 )
    {
        /*
        if ( showInfo) 
        {
            if( mSocketType == SOCK_STREAM )
            {
                printf( "Bytes received by Tcp: %d\n", iResult );
            }
            else if( mSocketType == SOCK_DGRAM )
            {
                printf( "Bytes received by Udp: %d\n", iResult );
            }
        }
        */
        //receiveBuffer[ iResult ] = '\0';
        mAlreadyReceived = true;
    }
    else 
    {
        #if SHOW_INFO
        printf( "Connection closed\n" );
        #endif
        ok &= false;
    }
    return ok;
}

bool Q4SSocket::shutDown( )
{
    int     iResult;
    bool    ok = true;

    // shutdown the connection since no more data will be sent
    if( mSocket > 0 )
    {
        #if SHOW_INFO
            printf( "Shutting down socket...\n" );
        #endif
        iResult = shutdown( mSocket, SHUT_WR);
        if( iResult < 0 )
        {
            //printf( "shutdown failed with error\n");
            //printf( "Error at socket()\n" );
            //printf("Error code: %d\n", errno);
        
            ok &= false;
        }
        disconnect( );
    }

    return ok;
}





bool Q4SSocket::disconnect( )
{
    bool    ok = true;

    // cleanup
    close(mSocket);

    return ok;
}