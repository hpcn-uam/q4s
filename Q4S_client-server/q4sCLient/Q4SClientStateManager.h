#include "Q4SClientState.h"

#include "Q4SClientProtocol.h"

class Q4SClientStateManager : Q4SClientProtocol
{

public:

    // Constructor-Destructor
    Q4SClientStateManager();
    ~Q4SClientStateManager();

    // Init-Done
    bool    init();
    void    done();

    bool    run();

private:

    void    clear();

    // State
    bool    stateInit   ( Q4SClientState state );
    void    stateDone   ( );

    bool stop;
    Q4SClientState q4SClientState;
    Q4SClientState nextState;
    Q4SSDPParams mParams;
    void    waitForLaunchGANY(); 
};