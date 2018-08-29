#include "Q4SMessageTools.h"

#include <sstream>

bool Q4SMessageTools_isPingMessage(std::string message, int *pingNumber, uint64_t *timeStamp)
{
    bool ok = true;

    // Auxiliar string
    std::string extracted;
    // Convert message to a stringstream 
    std::istringstream messageStream (message);

    // Get method from message
    if ( ok )
    {
        std::getline(messageStream, extracted, ' ');
        // Check if method is ping
        if ( extracted.compare("PING") != 0)
        {
            ok = false;
        }
    }

    // Discard rest of first line
    if (ok)
    {
        std::getline(messageStream, extracted);
    }

    // Get pingNumberfrom message
    if ( ok )
    {
        std::string sequenceNumberLineFirstPart;
        std::string sequenceNumberLineSecondPart;

        // Check text
        std::string sequenceNumberText = "Sequence-Number";
        getline( messageStream, sequenceNumberLineFirstPart, ':' );
        getline( messageStream, sequenceNumberLineSecondPart);
        if ( sequenceNumberLineFirstPart.compare( sequenceNumberText ) == 0)
        {
            *pingNumber = std::stoi(sequenceNumberLineSecondPart);
        }        
    }

    // Get TimeStamp
    if ( ok )
    {

        std::string timestampLineFirstPart;
        std::string timestampLineSecondPart;

        // Check text
        std::string timestampText = "Timestamp";
        getline( messageStream, timestampLineFirstPart, ':' );
        getline( messageStream, timestampLineSecondPart);
        if ( timestampLineFirstPart.compare( timestampText ) == 0)
        {
            //*timeStamp = std::strtoul(timestampLineSecondPart.c_str(), NULL, 0);
            sscanf(timestampLineSecondPart.c_str(), "%"PRIu64"", timeStamp);
        }
        
    }


    return ok;
}
bool Q4SMessageTools_is200OKMessage(std::string message,bool flagPing,  int *pingNumber, uint64_t *timeStamp)
{
    bool ok = true;

    // Auxiliar string
    std::string extracted;
    // Convert message to a stringstream 
    std::istringstream messageStream (message);

    // Get method from message
    if ( ok )
    {
        std::getline(messageStream, extracted, ' ');        
        std::getline(messageStream, extracted, ' ');


        // Check if method is ping
        //printf("%s\n", message.c_str());
        //printf("EXTRAIDO: %s\n", extracted.c_str());
        if ( extracted.compare("200") != 0)
        {
            //printf("No detecta\n");
            ok = false;
        }
    }

    // Discard rest of first line

   
    // Get pingNumberfrom message
    if ( ok && flagPing)
    {
        std::string sequenceNumberLineFirstPart;
        std::string sequenceNumberLineSecondPart;

        // Check text
        std::string sequenceNumberText = "Sequence-Number";
        getline( messageStream, sequenceNumberLineFirstPart, ':' );
        getline( messageStream, sequenceNumberLineSecondPart);
        if ( sequenceNumberLineFirstPart.compare( sequenceNumberText ) == 0)
        {
            *pingNumber = std::stoi(sequenceNumberLineSecondPart);
        }        
    }

    // Get TimeStamp
    if ( ok && flagPing)
    {

        std::string timestampLineFirstPart;
        std::string timestampLineSecondPart;

        // Check text
        std::string timestampText = "Timestamp";
        getline( messageStream, timestampLineFirstPart, ':' );
        getline( messageStream, timestampLineSecondPart);
        if ( timestampLineFirstPart.compare( timestampText ) == 0)
        {
            sscanf(timestampLineSecondPart.c_str(), "%"PRIu64"", timeStamp);
        }
        
    }


    return ok;
}
bool Q4SMessageTools_is200OKMessage(std::string message)
{
    bool ok = true;

    // Auxiliar string
    std::string extracted;

    // Convert message to a stringstream 
    std::istringstream messageStream (message);

    // Get method from message
    if ( ok )
    {
        std::getline(messageStream, extracted, ' ');
        std::getline(messageStream, extracted, ' ');
        // Check if method is ping
        if ( extracted.compare("200") != 0)
        {
            ok = false;
        }
    }

    // Discard rest of first line
    if (ok)
    {
        std::getline(messageStream, extracted);
    }

    return ok;
}
bool Q4SMessageTools_isBandWidthMessage(std::string message, int *sequenceNumber, int *BWpacket_size)
{
    bool ok = true;

    // Auxiliar string
    std::string extracted;

    // Convert message to a stringstream 


    std::istringstream messageStream (message);
    // Get method from message
    if ( ok )
    {
        std::getline(messageStream, extracted, ' ');
        // Check if method is ping
        if ( extracted.compare("BWIDTH") != 0)
        {
            ok = false;
        }
    }

    // Discard rest of first line
    if (ok)
    {
        std::getline(messageStream, extracted);
    }

    // Get pingNumberfrom message
    if ( ok )
    {
        std::string sequenceNumberLineFirstPart;
        std::string sequenceNumberLineSecondPart;
        std::string aux;        
        std::string body_msn;

        // Check text
        std::string sequenceNumberText = "Sequence-Number";
        getline( messageStream, sequenceNumberLineFirstPart, ':' );
        getline( messageStream, sequenceNumberLineSecondPart);
        if ( sequenceNumberLineFirstPart.compare( sequenceNumberText ) == 0)
        {
            *sequenceNumber = std::stoi(sequenceNumberLineSecondPart);
        }
        

        getline( messageStream, extracted,'\n'); 
        body_msn= messageStream.str();         
        getline( messageStream, extracted); 
        *BWpacket_size= extracted.size(); 
        


    }    
    return ok;
}

void Q4SMessageTools_fillBodyToASize(std::string &message, int size)
{
    int actualSize = message.size();

    int byteCounter;

    for (byteCounter=actualSize; byteCounter<size; byteCounter++)
    {
        std::string toAppend ="X";
        message.append(toAppend);
    }   
}
bool Q4SMessageTools_isCancel(std::string message)
{
    bool ok = false;

    // Auxiliar string
    std::string extracted;
    std::istringstream messageStream (message);

    // Convert message to a stringstream 
  
        std::getline(messageStream, extracted, '\n');
        // Check if method is Cancel
        if ( strcmp(extracted.c_str(), "CANCEL\r") == 0)
        {

            ok = true;
        }
    

    // Discard rest of first line
    if (ok)
    {
        std::getline(messageStream, extracted);
    }

    return ok;
}