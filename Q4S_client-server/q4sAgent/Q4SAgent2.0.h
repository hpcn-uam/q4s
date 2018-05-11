
#ifndef _Q4SAGENT2.0_H_
#define _Q4SAGENT2.0_H_

#include <string.h>
#include <sstream>
class Measure {
public:
	std::string  Jitter;
	int Latency;
	int PacketLoss;
};

extern Measure measure;

#endif // _Q4SAGENT2.0_H_