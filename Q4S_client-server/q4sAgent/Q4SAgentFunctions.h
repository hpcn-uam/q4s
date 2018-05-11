#ifndef _Q4SAGENTFUNCTIONS_H_
#define _Q4SAGENTFUNCTIONS_H_

#include <stdio.h>	

#include <stdlib.h>     
#include <iostream>
#include <string>
#include <sstream>     
#include <fstream>
#include <vector>
#include "Q4SAgentSocket.h"

using namespace std;


class Actuator{

	vector <string>				State;
	vector <string>				Nextstate;
	vector <string>				Action;
	vector <float>				LatencyMax;
	vector <float>				LatencyMin;
	//vector <int>				CondLatency; 
	vector <float>				JitterMax;
	vector <float>				JitterMin;
//	vector <int>				CondJitter;
	vector <float>				PacketlossMin;
	vector <float>				PacketlossMax;
//	vector <int>				CondPacketloss;
	vector <string>				TypeAlert;
	static string				CurrentState;
	int							CurrentValues;
	static int					CountSameStates;
	bool						UpdateJitter,UpdateLatency, UpdatePacketloss;
	Q4SAgentSocket              mAgentSocket;

	static float				inf;

public:
	//Actuator ();
	//~Actuator ();
	void						ReadConfigFile();
	void						Print ();
	void						SearchState (std::string);
	void						UpdateState ();
	void						TestRules (float, float, unsigned int );
	bool						PathAlert (float, float, unsigned int, std::string &, std::string);
	bool						PathRecovery (std::string &, std::string );

};

//extern Actuator actuador;




#endif //_Q4SAGENTFUNCTIONS_H_