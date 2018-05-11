#include <string.h>
#include <iostream>
#include <vector>


#include <stdio.h>

using namespace std;

#define DEBUG =0
#define VIRTUAL =0

#include "Q4SAgent2.h"


#include "Q4SAgentStateManager.h"
#include "Q4SAgentConfigFile.h"



int main(int argc, char *argv[]) 
{
	bool ok = true;
	Q4SAgentStateManager q4SAgentStateManager;
	actuator.ReadConfigFile();
	//actuator.Print();

	ok &= q4SAgentStateManager.init();
	
	

	sleep( q4SAgentConfigFile.timeEndApp);

    printf( "Saliendo de Server\n" );

    
	return 0;
}
