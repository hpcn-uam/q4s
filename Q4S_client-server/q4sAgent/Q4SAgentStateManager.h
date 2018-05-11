#ifndef _Q4SAGENTSTATEMANAGER_H_
#define _Q4SAGENTSTATEMANAGER_H_
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include "Q4SAgentState.h"

#include "Q4SAgentProtocol.h"

class Q4SAgentStateManager : Q4SAgentProtocol
{

public:

    // Constructor-Destructor
    Q4SAgentStateManager();
    ~Q4SAgentStateManager();

    // Init-Done
    bool    init();
    void    done();

private:

    void    clear();

    bool    run();

    // State
    bool    stateInit   ( Q4SAgentState state );
    void    stateDone   ( );

    bool stop;
    Q4SAgentState q4SAgentState;
    Q4SAgentState nextState;
    
};

#endif // _Q4SAGENTSTATEMANAGER_H_