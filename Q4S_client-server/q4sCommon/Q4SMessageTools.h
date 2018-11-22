#ifndef _Q4SMESSAGETOOLS_H_
#define _Q4SMESSAGETOOLS_H_
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <string>

bool Q4SMessageTools_isPingMessage(std::string message, int *pingNumber, uint64_t *timeStamp);
bool Q4SMessageTools_is200OKMessage(std::string message, bool flagPing, int *pingNumber, uint64_t *timeStamp);
bool Q4SMessageTools_isBandWidthMessage(std::string message, int *sequenceNumber, int *BWpacket_size);
bool Q4SMessageTools_isCancel(std::string message);
bool Q4SMessageTools_isErrorMessage(std::string message);
#endif //_Q4SMESSAGETOOLS_H_