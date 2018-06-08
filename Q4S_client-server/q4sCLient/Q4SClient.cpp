//#include "stdafx.h"

#include "Q4SClientStateManager.h"
#include <thread>         // std::thread
#include <stdio.h>
#include <mutex>          // std::mutex
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Q4SClientConfigFile.h"
bool GlobalFlag= false; 
std::mutex mtx; 
Q4SClientStateManager q4SClientStateManager;

void abrupt_exit(int param)
{	
	GlobalFlag= true; 
	q4SClientStateManager.done();   
	sleep(2);  
	exit(param);  
}

int main(int argc, char *argv[]) 
{
	bool ok = true;  
	void (*prev_handler)(int);
  	prev_handler= signal(SIGINT, abrupt_exit);

    //Q4SClientStateManager q4SClientStateManager;
    if (!GlobalFlag)
    {
	    ok &= q4SClientStateManager.init();
	    if (ok)
	    {
	        bool runOk = q4SClientStateManager.run();

	    }
	}
return 0; 
}