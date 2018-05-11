#ifndef ACTUATORFUNCTIONS_HPP_INCLUDED
#define ACTUATORFUNCTIONS_HPP_INCLUDED

	

#include <stdlib.h>     
#include <iostream>
#include <string>
#include <sstream>      // std::istringstream
#include <fstream>
#include <vector>

using namespace std;

class Actuator{
	vector <string> State;
	vector <string> Nextstate;
	vector <string> Action;
	vector <int> LatencyMax;
	vector <int> LatencyMin;
	vector <int> CondLatency; 
	vector <int> JitterMax;
	vector <int> JitterMin;
	vector <int> CondJitter;
	vector <int> Packetloss;
	vector <int> CondPacketloss;
	vector <string> TypeAlert;
	string CurrentState="S1";
	int CurrentValues;
public:
	void ReadConfigFile();
	void Print ();
	void SearchState ();
	void UpdateState ();
	void TestRules (int, int, int );

};


void Actuator::ReadConfigFile()
{
	cout << "Reading..."<< endl;
	ifstream config ("config.txt");
	std::string line;
	std::string key;	
	std::string value;
	    if ( config.is_open() )
	    {
	      	while( std::getline(config, line) )
	     	{
		        std::istringstream is_line(line);
		        if( std::getline(is_line, key, '#') ){
		        	State.push_back(key);
		        }
	          	if( std::getline(is_line, value,':') ) {
	           	 	LatencyMin.push_back(stoi(value));
	          	}
	          	if( std::getline(is_line, value,'@') ) {
	           	 	LatencyMax.push_back(stoi(value));
	          	} 
	          	if( std::getline(is_line, value,';') ) {
	           	 	CondLatency.push_back(stoi(value));
	          	}   
	          	if( std::getline(is_line, value,':') ) {
	           	 	JitterMin.push_back(stoi(value));
	          	}        
	          	if( std::getline(is_line, value,'@') ) {
	           	 	JitterMax.push_back(stoi(value));
          		} 
          		if( std::getline(is_line, value,';') ) {
	           	 	CondJitter.push_back(stoi(value));
	          	}   
          		if( std::getline(is_line, value,'@') ) {
	           	 	Packetloss.push_back(stoi(value));
          		}
          		if( std::getline(is_line, value,'#') ) {
	           	 	CondPacketloss.push_back(stoi(value));
	          	}   
          		if( std::getline(is_line, value,'#') ) {
	           	 	Action.push_back(value);
          		}
          		if( std::getline(is_line, value,'#') ) {
	           	 	Nextstate.push_back(value);
          		}
          		if( std::getline(is_line, value,' ') ) {
	           	 	TypeAlert.push_back(value);
          		}
	      	}
      		config.close();
	    }
	else  cout << "Unable to open file";  
}


void Actuator::Print (){

	for (int i=0; i<State.size();i++)
	{
		cout << "states"<< i << "  =" << State[i] << endl;
		cout << "LatencyMin"<< i << "  =" << LatencyMin[i] << endl;
		cout << "LatencyMax"<< i << "  =" << LatencyMax[i] << endl;
		cout << "JitterMin"<< i << "  =" << JitterMin[i] << endl;
		cout << "JitterMax"<< i << "  =" << JitterMax[i] << endl;
		cout << "Packetloss"<< i << "  =" << Packetloss[i] << endl;
		cout << "Action"<< i << "  =" << Action[i] << endl;
		cout << "Nextstate"<< i << "  =" << Nextstate[i] << endl;
	}
}

void Actuator::SearchState(){

	for (int i=0; i<State.size();i++){
		if ( State[i] == CurrentState){
			CurrentValues=i;
			break;
		}
	}
	cout << "CurrentValues esta en la posicion=> " << CurrentValues << endl;
}

void Actuator::UpdateState(){

	CurrentState=Nextstate[CurrentValues];
	cout << " Current State "<< CurrentState << endl;
}

void Actuator::TestRules (int JitterVirtual, int LatencyVirtual, int PacketlossVirtual){
#if DEBUG
	cout << "AQUI "<< TypeAlert[CurrentValues] << endl;
#endif
	if (TypeAlert[CurrentValues].compare("A") == 0){
		cout << "Alert " << endl;
	}
	else if (TypeAlert[CurrentValues].compare("R") == 0){
		cout << "Recovery " << endl;
	}
	else 
		cout << "Wrong Type Alert" << endl;

	//cout << "CondJitter[CurrentValues]=>" << CondJitter[CurrentValues]<< endl;

	switch (CondJitter[CurrentValues]){
		case 1:
			if (JitterVirtual > JitterMin[CurrentValues] && JitterVirtual<JitterMax[CurrentValues]){
				cout << "Case 1 JitterVirtual > JitterMin && JitterVirtual<JitterMin" << endl;
			}
			else{
				cout << " Case 1: no entro en el If"<< endl;
			}
		break;

		case 2:
			if (JitterVirtual<JitterMin[CurrentValues]){
				cout << "Case 2 JitterVirtual < JitterMin " << endl;
			}
			else{
				cout << " Case 2: no entro en el If"<< endl;
			}
		break;

		case 3:
			if (JitterVirtual>JitterMax[CurrentValues]){
				cout << "Case 3 JitterVirtual >JitterMax " << endl;
			}
			else{
				cout << " Case 3: no entro en el If"<< endl;
			}
		break;

		default:
		if (JitterVirtual > JitterMin[CurrentValues] && JitterVirtual<JitterMax[CurrentValues]){
				cout << "Case default JitterVirtual > JitterMin && JitterVirtual<JitterMin" << endl;
			}
			else{
				cout << " Case default: no entro en el If" << endl;
			}
	}

	switch (CondLatency[CurrentValues]){
		case 1:
			if (LatencyVirtual > LatencyMin[CurrentValues] && LatencyVirtual<LatencyMax[CurrentValues]){
				cout << "Case 1 LatencyVirtual > JitterMin && LatencyVirtual<JitterMin" << endl;
			}
			else{
				cout << " Case 1: no entro en el If"<< endl;
			}
		break;

		case 2:
			if (LatencyVirtual<LatencyMin[CurrentValues]){
				cout << "Case 2 LatencyVirtual < JitterMin " << endl;
			}
			else{
				cout << " Case 2: no entro en el If"<< endl;
			}
		break;

		case 3:
			if (LatencyVirtual>LatencyMax[CurrentValues]){
				cout << "Case 3 LatencyVirtual >JitterMax " << endl;
			}
			else{
				cout << " Case 3: no entro en el If"<< endl;
			}
		break;

		default:
		if (LatencyVirtual > LatencyMin[CurrentValues] && LatencyVirtual<LatencyMax[CurrentValues]){
				cout << "Case default LatencyVirtual > JitterMin && LatencyVirtual<JitterMin" << endl;
			}
			else{
				cout << " Case default: no entro en el If" << endl;
			}
	}

	switch (CondPacketloss[CurrentValues]){
		case 1:
			if (PacketlossVirtual == 0){
				cout << "Case 1 PacketlossVirtual==0" << endl;
			}
			else{
				cout << " Case 1: no entro en el If"<< endl;
			}
		break;

		case 2:
			if (PacketlossVirtual<Packetloss[CurrentValues]){
				cout << "Case 2 PacketlossVirtual < Packetloss[CurrentValues] " << endl;
			}
			else{
				cout << " Case 2: no entro en el If"<< endl;
			}
		break;

		case 3:
			if (PacketlossVirtual>Packetloss[CurrentValues]){
				cout << "Case 3 PacketlossVirtual>Packetloss[CurrentValues]" << endl;
			}
			else{
				cout << " Case 3: no entro en el If"<< endl;
			}
		break;

		default:
			if (PacketlossVirtual == 0){
				cout << "Case 1 PacketlossVirtual==0" << endl;
			}
			else{
				cout << " Case 1: no entro en el If"<< endl;
			}
	}
}




#endif