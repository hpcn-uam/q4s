#ifndef _Q4SMESSAGETOOLS_H_
#define _Q4SMESSAGETOOLS_H_
#include <string.h>

#include <string>

bool Q4SMessageTools_isPingMessage(std::string message, int *pingNumber, unsigned long *timeStamp);
bool Q4SMessageTools_is200OKMessage(std::string message);
bool Q4SMessageTools_isBandWidthMessage(std::string message, int *sequenceNumber);
void Q4SMessageTools_fillBodyToASize(std::string &message, int size);
bool Q4SMessageTools_isCancel(std::string message);

#endif //_Q4SMESSAGETOOLS_H_