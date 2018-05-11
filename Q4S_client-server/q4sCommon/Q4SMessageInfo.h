#ifndef _Q4SMESSAGEINFO_h_
#define _Q4SMESSAGEINFO_h_

#include <iostream>

#include <string>

// Info to store alongside with a received message.
struct Q4SMessageInfo 
{
    std::string     message;
    unsigned long   timeStamp;
};

#endif //_Q4SMESSAGEINFO_h_