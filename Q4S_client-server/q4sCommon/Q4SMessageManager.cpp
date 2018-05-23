#include "Q4SMessageManager.h"
#include "Q4SMessageTools.h"

Q4SMessageManager::Q4SMessageManager( )
{
    clear();
}

Q4SMessageManager::~Q4SMessageManager( )
{
    done();
}


bool Q4SMessageManager::init( )
{
    //done();

    bool ok = true;
    pthread_mutex_init(&mut_section, NULL);

    sem_init(&mevMessageReady, 0, 0);

    return ok;
}

void Q4SMessageManager::done( )
{
    mMessages.clear();
    pthread_mutex_destroy(&mut_section);
    sem_destroy(&mevMessageReady);
}

void Q4SMessageManager::clear( )
{

}


void Q4SMessageManager::addMessage( std::string &message, unsigned long timestamp )
{
    /*bool signal = false;
    
    

    if( mMessages.size( ) == 0 )
    {
        signal = true;
    }
    */
    token++;
    Q4SMessageInfo messageInfo;
    messageInfo.message = message;
    messageInfo.timeStamp = timestamp;    
    pthread_mutex_lock (&mut_section);    
    sem_post( &mevMessageReady);
    mMessages.push_back( messageInfo );
    pthread_mutex_unlock (&mut_section);
}

bool Q4SMessageManager::readFirst( std::string &firstMessage )
{
    bool    ok = true;
    

    sem_wait( &mevMessageReady);
    token=token-1;
    pthread_mutex_lock (&mut_section);
    
    if( mMessages.size( ) == 0 )
    {
        printf( "FATAL ERROR. Message available but stolen by another thread.\n" );
        ok = false;
    }
    else
    {
        firstMessage = mMessages.front().message;
        mMessages.pop_front();
        if( mMessages.size( ) == 0 )
        {
        }
    }
    pthread_mutex_unlock (&mut_section);

    return ok;
}
bool Q4SMessageManager::readmMessages()
{
    bool ok= true; 
    std::string firstMessage;
    firstMessage = mMessages.front().message;
    return ok;
}
bool Q4SMessageManager::readMessage( std::string& pattern, Q4SMessageInfo& messageInfo, bool erase )
{
    bool    found = false;
    std::list< Q4SMessageInfo >::iterator  itr_msg;
    sem_wait( &mevMessageReady);
    token=token-1;
    pthread_mutex_lock (&mut_section);



    for( itr_msg = mMessages.begin( ); ( found == false ) && ( itr_msg != mMessages.end( ) ); itr_msg++ )
    {
        if( itr_msg->message.substr( 0, pattern.size( ) ).compare( pattern ) == 0 )
        {
            // Message found.
            found = true;
            messageInfo.message = itr_msg->message;
            messageInfo.timeStamp = itr_msg->timeStamp;
        }
    }

    if (erase && found)
    {
        itr_msg--;

        mMessages.erase(itr_msg);
        if( mMessages.size( ) == 0 )
        {
        }
    } 
    if(!found) 
    {
        sem_post( &mevMessageReady);
        token++;
    }
    pthread_mutex_unlock (&mut_section);

    return found;
}

bool Q4SMessageManager::readPingMessage( int pingIndex, Q4SMessageInfo& messageInfo, bool erase )
{
    bool    found = false;
    std::list< Q4SMessageInfo >::iterator  itr_msg;
    sem_wait( &mevMessageReady);
    token=token-1;
    pthread_mutex_lock (&mut_section);


    for( itr_msg = mMessages.begin( ); ( found == false ) && ( itr_msg != mMessages.end( ) ); itr_msg++ )
    {
        
        int messagePingIndex;
        unsigned long timeStamp;
        if (Q4SMessageTools_isPingMessage(itr_msg->message, &messagePingIndex, &timeStamp))
        {
           if (messagePingIndex == pingIndex)
            {
                // Message found.
                found = true;
                messageInfo.message = itr_msg->message;
                messageInfo.timeStamp = itr_msg->timeStamp;
            }
        }
    }

    if (erase && found)
    {
        itr_msg--;
        mMessages.erase(itr_msg);
        if( mMessages.size( ) == 0 )
        {
        }
    } 
    
    if(!found || !erase) 
    {
        token++;
        sem_post( &mevMessageReady);
    }

    pthread_mutex_unlock (&mut_section);

    return found;
}
bool Q4SMessageManager::read200OKMessage( Q4SMessageInfo& messageInfo, bool erase )
{
    bool    found = false;
    std::list< Q4SMessageInfo >::iterator  itr_msg;

    sem_wait( &mevMessageReady);
    token=token-1;
    pthread_mutex_lock (&mut_section);


    for( itr_msg = mMessages.begin( ); ( found == false ) && ( itr_msg != mMessages.end( ) ); itr_msg++ )
    {
       if (Q4SMessageTools_is200OKMessage(itr_msg->message))
        {
            // Message found.
            found = true;
            messageInfo.message = itr_msg->message;
            messageInfo.timeStamp = itr_msg->timeStamp;
        }
    }
    if (erase && found)
    {
        itr_msg--;
        mMessages.erase(itr_msg);
        if( mMessages.size( ) == 0 )
        {
        }
    } 
    if(!found)
    {        

        sem_post( &mevMessageReady);
        token++;
    }

    pthread_mutex_unlock (&mut_section);

    return found;
}

bool Q4SMessageManager::readBandWidthMessage(unsigned long &sequenceNumber, bool erase, unsigned long *timestampBW)
{
    bool    found = false;
    std::list< Q4SMessageInfo >::iterator  itr_msg;
    sem_wait( &mevMessageReady);    
    token=token-1;
    pthread_mutex_lock (&mut_section);

    /*
    */
    for( itr_msg = mMessages.begin( ); ( found == false ) && ( itr_msg != mMessages.end( ) ); itr_msg++ )
    {
       int messagePingIndex;
       if (Q4SMessageTools_isBandWidthMessage(itr_msg->message, &messagePingIndex))
       {
            // Message found.
            found = true;
           sequenceNumber = messagePingIndex;
           *timestampBW= itr_msg->timeStamp; 
           //printf("%lu\n", itr_msg->timeStamp);
       }
    }
   
    if (erase && found)
    {
        itr_msg--;
        mMessages.erase(itr_msg);
        if( mMessages.size( ) == 0 )
        {
    
            found=false; 
        }
    } 
    else if(!found || !erase) 
    {
        sem_post( &mevMessageReady);
        token++;
    }

    pthread_mutex_unlock (&mut_section);

    return found;
}

int Q4SMessageManager::size() const
{
    return mMessages.size();
}

bool Q4SMessageManager::readCancelMessage()
{
    bool    found = false;
    std::list< Q4SMessageInfo >::iterator  itr_msg;
    
    pthread_mutex_lock (&mut_section);


    for( itr_msg = mMessages.begin( ); ( found == false ) && ( itr_msg != mMessages.end( ) ); itr_msg++ )
    {
       if (Q4SMessageTools_isCancel(itr_msg->message))
        {
            // Message found.
            found = true;
            printf("CANCEL ENCONTRADO\n");
        }
    }
    
    pthread_mutex_unlock (&mut_section);

    return found;
}
