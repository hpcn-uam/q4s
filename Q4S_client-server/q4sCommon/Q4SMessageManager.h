#include <pthread.h>
#include <semaphore.h>
//#include "Q4SCriticalSection.h"
#include "Q4SMessageInfo.h"

#include <list>
#include <string>

class Q4SMessageManager 
{
    public:

    // Constructor-Destructor
    Q4SMessageManager( );
    ~Q4SMessageManager( );
     
    // Init-Done
    bool    init( );
    void    done( );

    // Add message
    void addMessage( std::string &message, uint64_t timestamp = 0 );
    
    bool readCancelMessage();
    bool readFirst( std::string &firstMessage );
    bool readMessage( std::string& pattern, Q4SMessageInfo& messageInfo, bool erase );
    bool readPingMessage( int pingIndex, Q4SMessageInfo& messageInfo, bool erase );
    bool read200OKMessage( Q4SMessageInfo& messageInfo, bool erase, uint64_t *TimestampPing, int *sequenceNumberPing  );
    bool readBandWidthMessage(unsigned long &sequenceNumber, bool erase, uint64_t *timestampBW, int *BWpacket_size);
    bool readmMessages();
    int size() const;
    bool eraseMessages();

private:
    
    void    clear( );

    pthread_mutex_t                 mut_section;
    std::list < Q4SMessageInfo >    mMessages;
    sem_t                           mevMessageReady;


    //////////////////////////////
    int token = 0; 
};

