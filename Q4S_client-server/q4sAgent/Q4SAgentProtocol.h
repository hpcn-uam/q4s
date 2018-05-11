#ifndef _Q4SAGENTPROTOCOL_H_
#define _Q4SAGENTPROTOCOL_H_
#include "../q4sCommon/ConfigFile.h"
#include "Q4SAgentSocket.h"
#include "../q4sCommon/Q4SMessageManager.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>


class Q4SAgentProtocol
{
public:

    // Constructor-Destructor
    Q4SAgentProtocol ();
    ~Q4SAgentProtocol ();

	template<size_t N>
	std::string convert(char const(&data)[N])
	{
	   return std::string(data, std::find(data, data + N, '\0'));
	}


	template <typename ValueType>
    ValueType getValueOfKey(const std::string &key, ValueType const &defaultValue = ValueType()) const
    {
	    if (contents.find(key) != contents.end())
		    return defaultValue;

	    return Convert::string_to_T<ValueType>(contents.find(key)->second);
    }


    // Init-Done
    bool    init();
    void    done();

    // Q4S Methods
	void*	listen();
    bool    Startlisten();
    void    end();

private:

    void    clear();

    bool    openConnectionListening();
	bool	openListen();
    void    closeConnectionListening();
    void    closeConnections();

    Q4SAgentSocket                mAgentSocket;
    pthread_t                     marrthrListenHandle;
	pthread_t                     marrthrListenHandle2;
    pthread_t                     marrthrDataHandle;

    void*                       manageUdpConnection( );
    static void*         		manageUdpConnectionsFn( void* lpData );

    void*                        manageUdpReceivedData( );
    static void*		         manageUdpReceivedDataFn( void* lpData );

	static void*				listenFn( void* lpData );

    Q4SMessageManager           mReceivedMessages;

	std::map<std::string, std::string> contents;
	

};




#endif // _Q4SAGENTPROTOCOL_H_