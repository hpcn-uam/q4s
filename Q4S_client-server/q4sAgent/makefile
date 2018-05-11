CC=g++
HEADERS_COMMON=../q4sCommon/ConfigFile.h ../q4sCommon/Q4SCommonProtocol.h ../q4sCommon/Q4SMessageManager.h ../q4sCommon/Q4SMessageInfo.h ../q4sCommon/Q4SCriticalSection.h ../q4sCommon/Q4SMessageTools.h ../q4sCommon/Q4SSocket.h ../q4sCommon/Q4SMathUtils.h ../q4sCommon/Q4SStructs.h ../q4sCommon/Q4SMessage.h ../q4sCommon/Q4SSDP.h ../q4sCommon/Q4SSDPParams.h

HEADERS_AGENT=Q4SAgent2.h Q4SAgentProtocol.h  Q4SAgentSocket.h  Q4SAgentStateManager.h Q4SAgentState.h Q4SAgentFunctions.h Q4SAgentConfigFile.h actuatorfunctions.h 

FILE_COMMON= ../q4sCommon/ConfigFile.cpp ../q4sCommon/Q4SCommonProtocol.cpp ../q4sCommon/Q4SMessageManager.cpp ../q4sCommon/Q4SCriticalSection.cpp ../q4sCommon/Q4SSocket.cpp ../q4sCommon/Q4SMathUtils.cpp ../q4sCommon/Q4SMessage.cpp ../q4sCommon/Q4SSDP.cpp ../q4sCommon/Q4SStructs.cpp ../q4sCommon/Q4SSDPParams.cpp ../q4sCommon/Q4SMessageTools.cpp

FILE_AGENT= Q4SAgentConfigFile.cpp Q4SAgent2.cpp  Q4SAgentProtocol.cpp Q4SAgentSocket.cpp Q4SAgentStateManager.cpp Q4SAgentFunctions.cpp
DEBUG_FLAGS= -DDEBUG
OBJ=Q4SAgent
FLAGS= -pthread
all:
	$(CC) $(HEADERS_COMMON) $(HEADERS_AGENT) $(FILE_COMMON) $(FILE_AGENT) $(DEBUG_FLAGS) $(FLAGS) -o $(OBJ)
clean:
	rm -rf $(OBJ) *~* 
