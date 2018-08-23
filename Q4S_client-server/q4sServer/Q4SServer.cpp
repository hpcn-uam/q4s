#include "Q4SServerStateManager.h"


int main(int argc, char *argv[]) 
{
	bool ok;
    Q4SServerStateManager q4SServerStateManager;


	ok = true;
	ok &= q4SServerStateManager.init();

	if (ok)
	{
		q4SServerStateManager.run();
	}

    

    return 0;
}