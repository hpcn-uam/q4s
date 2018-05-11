#include <pthread.h>

class Q4SCriticalSection
{
    public:

        Q4SCriticalSection( )                               { clear( );  }
        ~Q4SCriticalSection( )                              { done( );   }


        // Init-Done

        bool    init     ( int cfgMode = FMCS_CSMODE );
        void    done     ( );


        // Enter-Leave

        void    enter    ( ) const;
        void    leave    ( ) const;

        enum    Mode { FMCS_CSMODE = 0, FMCS_MTXMODE };

        mutable pthread_mutex_t mut_section;

  //----------------------------------------------------------------------------------------------------

    private:
 
        void    clear( );

        bool                         ok;
        //HANDLE                       mutex;
        int                          mode;
};


