CC=g++
HEADERS_COMMON=../q4sCommon/ConfigFile.h ../q4sCommon/Q4SCommonProtocol.h ../q4sCommon/Q4SMessageManager.h ../q4sCommon/Q4SMessageInfo.h ../q4sCommon/Q4SCriticalSection.h ../q4sCommon/Q4SMessageTools.h ../q4sCommon/Q4SSocket.h ../q4sCommon/Q4SMathUtils.h ../q4sCommon/Q4SStructs.h ../q4sCommon/Q4SMessage.h ../q4sCommon/Q4SSDP.h ../q4sCommon/Q4SSDPParams.h 
HEADERS_SERVER=Q4SServerConfigFile.h Q4SServerProtocol.h Q4SServerSocket.h Q4SServerState.h Q4SServerStateManager.h 
FILE_COMMON= ../q4sCommon/ConfigFile.cpp ../q4sCommon/Q4SCommonProtocol.cpp ../q4sCommon/Q4SMessageManager.cpp ../q4sCommon/Q4SCriticalSection.cpp ../q4sCommon/Q4SSocket.cpp ../q4sCommon/Q4SMathUtils.cpp ../q4sCommon/Q4SMessage.cpp ../q4sCommon/Q4SSDP.cpp ../q4sCommon/Q4SStructs.cpp ../q4sCommon/Q4SSDPParams.cpp ../q4sCommon/Q4SMessageTools.cpp
FILE_SERVER=Q4SServerConfigFile.cpp Q4SServer.cpp Q4SServerProtocol.cpp Q4SServerSocket.cpp Q4SServerStateManager.cpp
DEBUG_FLAGS= -DDEBUG
OBJ=Q4SServer
FLAGS= -pthread -Wall -ggdb3
ifeq ($(SHOW_INFO), 1)  #at this point, the makefile checks if FEATURE is enabled
SHOW1 = -DSHOW_INFO 
endif
ifeq ($(SHOW_INFO2), 1)  #at this point, the makefile checks if FEATURE is enabled
SHOW2 = -DSHOW_INFO2 
endif
ifeq ($(SAVE_INFO), 1)  #at this point, the makefile checks if FEATURE is enabled
SAVE = -DSAVE_INFO 
endif
all:
	$(CC) $(HEADERS_COMMON) $(HEADERS_SERVER) $(FILE_COMMON) $(FILE_SERVER) $(DEBUG_FLAGS) $(FLAGS) -o $(OBJ) $(SHOW1) $(SHOW2) $(SAVE)
clean:
	rm -rf $(OBJ) *~* 
