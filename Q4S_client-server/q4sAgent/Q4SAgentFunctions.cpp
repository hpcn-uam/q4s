#include "Q4SAgentFunctions.h"

#include <limits>

#include <stdio.h>	
#include <stdlib.h>     
#include <iostream>
#include <string>
#include <sstream>      
#include <fstream>
#include <vector>


int	   Actuator::CountSameStates=0;
string Actuator::CurrentState ="S0";
float  Actuator::inf = std::numeric_limits<float>::infinity();

/*Actuator::Actuator ()
{
    clear();
}
Actuator::~Actuator ()
{
    done();
}*/

void Actuator::ReadConfigFile()
{
	//cout << "Reading..."<< endl;
	ifstream config ("Rules.csv");
	std::string line;
	std::string key;	
	std::string value;
	    if ( config.is_open() )
	    {
	      	while( std::getline(config, line) )
	     	{
		        std::istringstream is_line(line);
		        if( std::getline(is_line, key, ';') ){
		        	State.push_back(key);
		        }
				if( std::getline(is_line, value,';') ) {
	           	 	TypeAlert.push_back(value);
          		}
	          	if( std::getline(is_line, value,';') ) {
	           	 	LatencyMin.push_back(stof(value));
	          	}
	          	if( std::getline(is_line, value,';') ) {
					if(value.compare("*")!=0)
	           	 		LatencyMax.push_back(stof(value));
					else
						LatencyMax.push_back(inf);
	          	}   
	          	if( std::getline(is_line, value,';') ) {
	           	 	JitterMin.push_back(stof(value));
	          	}        
	          	if( std::getline(is_line, value,';') ) {
					if(value.compare("*")!=0)
	           	 		JitterMax.push_back(stof(value));
					else
						JitterMax.push_back(inf);
          		}  
          		if( std::getline(is_line, value,';') ) {
	           	 	PacketlossMin.push_back(stof(value));
          		} 
          		if( std::getline(is_line, value,';') ) {
					if(value.compare("*")!=0)
	           	 		PacketlossMax.push_back(stof(value));
					else
						PacketlossMax.push_back(inf);
          		} 
          		if( std::getline(is_line, value,';') ) {
	           	 	Action.push_back(value);
          		}
          		if( std::getline(is_line, value,' ') ) {
	           	 	Nextstate.push_back(value);
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
		cout << "PacketlossMin"<< i << "  =" << PacketlossMin[i] << endl;
		cout << "PacketlossMax"<< i << "  =" << PacketlossMax[i] << endl;
		cout << "Action"<< i << "  =" << Action[i] << endl;
		cout << "Nextstate"<< i << "  =" << Nextstate[i] << endl;
	}
}

void Actuator::SearchState(std::string TypeAlertIn){

	bool FirstTime=true;
	for (int i=0; i<State.size();i++){
		if ( State[i] == CurrentState && TypeAlert[i]==TypeAlertIn){
			if (FirstTime){
				CurrentValues=i;
				FirstTime=false;
			}
			//break;
			CountSameStates++;
		}
	}
	//cout << "CurrentValues esta en la posicion=> " << CurrentValues << endl;
}

void Actuator::UpdateState(){
	
	cout << "Current State: "<< Nextstate[CurrentValues] << " - Previous State: " << State[CurrentValues] << endl;
	CurrentState=Nextstate[CurrentValues];
	//cout << "Current State "<< CurrentState << endl;
}

void Actuator::TestRules (float Jitter, float Latency, unsigned int Packetloss){
#if DEBUG
	cout << "AQUI "<< TypeAlert[CurrentValues] << endl;
#endif
	for (int i=0;i<CountSameStates;i++){
		if (Jitter > JitterMin[CurrentValues+i] && Jitter<JitterMax[CurrentValues+i]){
			UpdateJitter=false;
		}
		else{
			UpdateJitter=true;
			CurrentValues=CurrentValues+i;
			break;
		}
		
		if (Latency > LatencyMin[CurrentValues+i] && Latency<LatencyMax[CurrentValues+i]){
			UpdateLatency=false;
		}
		else{
			UpdateLatency=true;
			CurrentValues=CurrentValues+i;
			break;
		}
		//if (Packetloss == 0){(Packetloss > PacketlossMin[CurrentValues+i] && Packetloss<PacketlossMax[CurrentValues+i])
		if (Packetloss > PacketlossMin[CurrentValues+i] && Packetloss<PacketlossMax[CurrentValues+i]){
			UpdatePacketloss=false;
		}
		else{
			UpdatePacketloss=true;
			CurrentValues=CurrentValues+i;
			break;
		}
	}
}



bool Actuator::PathAlert (float Jitter, float Latency, unsigned int Packetloss, std::string &action, std::string TypeAlerIn )
{
	bool ok=true;
	//printf("Path\n");
		
	SearchState( TypeAlerIn);

	TestRules (Jitter , Jitter , 0);

	if (UpdateJitter==true || UpdateLatency==true || UpdatePacketloss==true)
	{
		UpdateState();
	}
	action=Action[CurrentValues];
	return ok;
}

bool Actuator::PathRecovery (std::string &action, std::string TypeAlerIn )
{
	bool ok=true;
	//printf("Path\n");
		
	SearchState(TypeAlerIn);
	UpdateState();
	
	action=Action[CurrentValues];
	return ok;
}