#include "Q4SMessageTools.h"

#include <sstream>

bool Q4SMessageTools_isPingMessage(std::string message, int *pingNumber, uint64_t *timeStamp)
{
    bool ok = true;
    bool HeaderEnd=false;
    // Auxiliar string
    std::string extracted;
    std::string::size_type initialPosition;
    std::string::size_type finalPosition;
    std::string patternSeq="Sequence-Number:";
    std::string patternCL="Content-Length:";
    std::string patternTS="Timestamp:";
    // Convert message to a stringstream 

    std::istringstream messageStream (message);
    std::getline(messageStream, extracted);
    initialPosition = extracted.find("PING");
    if (initialPosition == std::string::npos)
    {
        ok = false;
    }
    // Get method from message
    if ( ok )
    {
        while (!extracted.empty()&&!HeaderEnd)
        {
            extracted={}; 
            std::getline(messageStream, extracted);

            initialPosition = extracted.find(patternSeq);
            if (initialPosition != std::string::npos)
            {
                initialPosition = initialPosition + patternSeq.length();
                finalPosition = extracted.find("\n", initialPosition+1);
                *pingNumber = std::stoi(extracted.substr(initialPosition, finalPosition-initialPosition));

            }
            initialPosition = extracted.find(patternTS);
            if (initialPosition != std::string::npos)
            {
                initialPosition = initialPosition + patternTS.length();
                finalPosition = extracted.find("\n", initialPosition+1);

                sscanf(extracted.substr(initialPosition, finalPosition-initialPosition).c_str(), "%" PRIu64 "", timeStamp);


            }
            
            if (strcmp(extracted.c_str(),"\r")==0)
            {
                HeaderEnd=true;   
            }

        }

        //printf("SEQUENCE %d\n", *sequenceNumber);
        
    }
  
    return ok;
}
bool Q4SMessageTools_is200OKMessage(std::string message,bool flagPing,  int *pingNumber, uint64_t *timeStamp)
{

    bool ok = true;
    bool HeaderEnd=false;
    // Auxiliar string
    std::string extracted;
    std::string::size_type initialPosition;
    std::string::size_type finalPosition;
    std::string patternSeq="Sequence-Number:";
    std::string patternCL="Content-Length:";
    std::string patternTS="Timestamp:";
    // Convert message to a stringstream 

    std::istringstream messageStream (message);
    std::getline(messageStream, extracted);
    initialPosition = extracted.find("200");
    if (initialPosition == std::string::npos)
    {
        ok = false;
    }
    // Get method from message
    if ( ok )
    {
        while (!extracted.empty()&&!HeaderEnd&&flagPing)
        {
            extracted={}; 
            std::getline(messageStream, extracted);

            initialPosition = extracted.find(patternSeq);
            if (initialPosition != std::string::npos)
            {
                initialPosition = initialPosition + patternSeq.length();
                finalPosition = extracted.find("\n", initialPosition+1);
                *pingNumber = std::stoi(extracted.substr(initialPosition, finalPosition-initialPosition));
            }
            initialPosition = extracted.find(patternTS);
            if (initialPosition != std::string::npos)
            {
                initialPosition = initialPosition + patternTS.length();
                finalPosition = extracted.find("\n", initialPosition+1);
                sscanf(extracted.substr(initialPosition, finalPosition-initialPosition).c_str(), "%" PRIu64 "", timeStamp);
            }
            
            if (strcmp(extracted.c_str(),"\r")==0)
            {
                HeaderEnd=true;   
            }

        }
        //printf("SEQUENCE %d\n", *sequenceNumber);
        
    }
  
    return ok;
}
bool Q4SMessageTools_isErrorMessage(std::string message)
{

    bool ok = false;

    std::string extracted;
    std::string::size_type initialPosition;
    // Auxiliar string
    
    // Convert message to a stringstream 

    std::istringstream messageStream (message);
    std::getline(messageStream, extracted);
    initialPosition = extracted.find("400");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok; 
    }
    // Get method from message
    initialPosition = extracted.find("404");

    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("405");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("406");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("408");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("413");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("414");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("415");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("416");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("500");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    // Get method from message
    initialPosition = extracted.find("501");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("503");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("504");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("505");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("513");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("600");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("601");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("603");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    }
    initialPosition = extracted.find("604");
    if (initialPosition == std::string::npos)
    {
        ok = true;
        return ok;
    } 
    return ok;
}
/*
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
*/
bool Q4SMessageTools_isBandWidthMessage(std::string message, int *sequenceNumber, int *BWpacket_size)
{
    bool ok = true;
    bool HeaderEnd=false;
    // Auxiliar string
    std::string extracted;
    std::string::size_type initialPosition;
    std::string::size_type finalPosition;
    std::string patternSeq="Sequence-Number:";
    std::string patternCL="Content-Length:";
    // Convert message to a stringstream 
    *sequenceNumber=0; 
    *BWpacket_size=0;
    std::istringstream messageStream (message);
    std::getline(messageStream, extracted);
    initialPosition = extracted.find("BWIDTH");
    if (initialPosition == std::string::npos)
    {
        ok = false;
    }
    // Get method from message
    if ( ok )
    {
        while (!extracted.empty()&&!HeaderEnd)
        {
            extracted={}; 
            std::getline(messageStream, extracted);

            initialPosition = extracted.find(patternSeq);
            if (initialPosition != std::string::npos)
            {
                initialPosition = initialPosition + patternSeq.length();
                finalPosition = extracted.find("\n", initialPosition+1);
                *sequenceNumber = std::stoi(extracted.substr(initialPosition, finalPosition-initialPosition));
            }
            initialPosition = extracted.find(patternCL);
            if (initialPosition != std::string::npos)
            {
                initialPosition = initialPosition + patternCL.length();
                finalPosition = extracted.find("\n", initialPosition+1);
                *BWpacket_size = std::stoi(extracted.substr(initialPosition, finalPosition-initialPosition));

            }
            
            if (strcmp(extracted.c_str(),"\r")==0)
            {
                HeaderEnd=true;   
            }

        }
        if(*BWpacket_size==0)
        {
            std::getline(messageStream, extracted);
            *BWpacket_size= extracted.size(); 
            
                
        }
        //printf("SEQUENCE %d\n", *sequenceNumber);
        
    }
  
    return ok;
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