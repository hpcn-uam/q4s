

#include "../q4sCommon/Q4SSocket.h"

#include <stdio.h>
#include <list>

class Q4SServerSocket
{
public:

    // Constructor-Destructor
    Q4SServerSocket( );
    ~Q4SServerSocket( );

    // Init-Done
    bool    init( );
    void    done( );



    bool    startTcpListening();
    bool    waitForTcpConnection( int connectionId );
    bool    stopWaiting( );
    bool    sendTcpData( int connId, const char* sendBuffer );
    bool    receiveTcpData( int connectionId, char* receiveBuffer, int receiveBufferSize );

    bool    startUdpListening( );
    bool    sendUdpData( int connectionId, const char* sendBuffer );
    bool    receiveUdpData( char* receiveBuffer, int receiveBufferSize, int& connectionId );

    bool    startAlertSender( );
    bool    sendAlertData( const char* sendBuffer );

    bool    closeConnection( int socketType );



private:

	struct Q4SConnectionInfo 
    {
        int             id;
        Q4SSocket       q4sTcpSocket;
        sockaddr_in     peerTcpAddrInfo;
        sockaddr_in     peerUdpAddrInfo;
    };
	void clear();

    bool    initializeSockets();

    bool    createListenSocket( );
    bool    bindListenSocket( );
    bool    startListen( );
    bool    acceptClientConnection( Q4SConnectionInfo* connectionInfo );
    bool    closeListenSocket( );

    bool    createUdpSocket( );
    bool    bindUdpSocket( );

    bool   getTcpSocket( int connectionId, Q4SSocket*& pQ4SSocket );
    bool    getConnectionInfo( int connectionId, Q4SConnectionInfo*& pQ4SConnInfo );
    bool    getConnectionInfo( sockaddr_in& connectionInfo, Q4SConnectionInfo*& pQ4SConnInfo );

    bool   createAlertUdpSocket();

 
    int                 mListenSocket;
    struct addrinfo*    mpAddrInfoResultTcp; 
    std::list<Q4SConnectionInfo*>   listConnectionInfo;

    int                 mUdpSocket;
    struct addrinfo*    mpAddrInfoResultUdp; 
    Q4SSocket           mq4sUdpSocket;

	int 				mAlertSocket;
    struct addrinfo*    mpAddrInfoResultAlert; 
	Q4SSocket           mq4sAlertSocket;



};