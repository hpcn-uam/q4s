#include "Q4SServerStateManager.h"
#include "Q4SServerConfigFile.h"


int main(int argc, char *argv[]) 
{
	bool ok = true;

    Q4SServerStateManager q4SServerStateManager;

    ok &= q4SServerStateManager.init();
    printf("Server\n"); 
    
    if (ok)
    {
        q4SServerStateManager.run();
    }
    sleep( (int)q4SServerConfigFile.timeEndApp/1000);

    printf( "Saliendo de Server\n" );
    system("pause");

    return 0;
}