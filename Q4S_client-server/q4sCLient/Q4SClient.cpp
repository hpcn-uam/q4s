//#include "stdafx.h"

#include "Q4SClientStateManager.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Q4SClientConfigFile.h"

int main(int argc, char *argv[]) 
{
	bool ok = true;

    Q4SClientStateManager q4SClientStateManager;

    ok &= q4SClientStateManager.init();
    if (ok)
    {
        bool runOk = q4SClientStateManager.run();

    }
}