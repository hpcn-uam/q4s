# Q4S

## Description

This project provides an implementation of [Quality for Service Protocol (Q4S)](https://datatracker.ietf.org/doc/draft-aranda-dispatch-q4s/ "Q4S @ IETF Datatracker"). This implementation has been developed at HPCN-UAM, in the context of the [Racing Drones project](http://www.hpcn-uam.es/project/racing-drones/ "Racing Drones @ HPCN-UAM"). Current version is compatible with Linux, and is based on the [OptivaMedia's project](https://github.com/OptivaMediaArqueopterix/q4s).

## Instructions

Q4S follows a client-server architecture. We provide two different makefiles, one for the server side and the other for the client, to simplify their distribution. Once you have downloaded the repository, follow the instructions to build and execute each part.

### Build process

#### Option 1: 
You can go to the folder ```q4s/Q4S_client-server/q4sCLient/``` or ```q4s/Q4S_client-server/q4sServer/``` and execute ```make``` to build the client or server part, respectively.

#### Option 2:
Use the script ```execute_client.sh``` or ```execute_server.sh``` and select the option "Compile". You can set the flags to provide different debug and log levels.

### Execution

#### Option 1: 
Set the options for client and server sides in their respective configuration files (i.e., ```q4s/Q4S_client-server/q4sCLient/Q4SClientConfig.txt``` and ```q4s/Q4S_client-server/q4sServer/Q4SServerConfig.txt```, respectively) and execute the binaries after compilation.

#### Option 2: 
Set the options for client and server sides in their respective configuration files (i.e., ```q4s/Q4S_client-server/q4sCLient/Q4SClientConfig.txt``` and ```q4s/Q4S_client-server/q4sServer/Q4SServerConfig.txt```, respectively) and use the script ```execute_client.sh``` or ```execute_server.sh``` with the option "Run".

### Observation:
In the configuration for Q4S client, the option ```READY_BW``` allows enabling or disabling the measurements of bandwidth before Continuity (1 enabled, 2 disabled).
