#include "ushell_core_utils.h"
#include "ushell_user_logger.h"
#include <stdint.h>

/*
Note:
    The definitios of num8_t, num16_t num32_t, num64_t are declared here:
    ..sources\ushell_settings\inc\ushell_core_settings.h
    and can be extended or adapted e.g. to signed variants, according to the user's needs
*/

///////////////////////////////////////////////////////////////////
//                  USER'S FUNCTIONS                             //
///////////////////////////////////////////////////////////////////


/*---------------------------------------------------------------*/
int vtest(void)
{
    uSHELL_LOG(LOG_VERBOSE, "--> vtest()" );

    return 0;
}

/*---------------------------------------------------------------*/
int itest(uint32_t i)
{
    uSHELL_LOG(LOG_VERBOSE, "--> itest()" );
    uSHELL_LOG(LOG_INFO, "i = %u", i );

    return 0;
}

/*---------------------------------------------------------------*/
int stest(char *s)
{
    uSHELL_LOG(LOG_VERBOSE, "--> stest()" );
    uSHELL_LOG(LOG_INFO, "s = %s", s );

    return 0;
}

/*---------------------------------------------------------------*/
int iitest(uint32_t i1, uint32_t i2)
{
    uSHELL_LOG(LOG_VERBOSE, "--> iitest()" );
    uSHELL_LOG(LOG_INFO, "i1 = %d", i1 );
    uSHELL_LOG(LOG_INFO, "i2 = %d", i2 );

    return 0;
}

/*---------------------------------------------------------------*/
int istest(uint32_t i, char *s)
{
    uSHELL_LOG(LOG_VERBOSE, "--> istest()" );
    uSHELL_LOG(LOG_INFO, "i = %d", i );
    uSHELL_LOG(LOG_INFO, "s = %s", s );

    return 0;
}

/*---------------------------------------------------------------*/
int sstest(char *s1, char *s2)
{
    uSHELL_LOG(LOG_VERBOSE, "--> sstest()" );
    uSHELL_LOG(LOG_INFO, "s1 = %s", s1 );
    uSHELL_LOG(LOG_INFO, "s2 = %s", s2 );

    return 0;
}

/*---------------------------------------------------------------*/
int liotest(uint64_t l, uint32_t i, bool o)
{
    uSHELL_LOG(LOG_VERBOSE, "--> liotest()" );
    uSHELL_LOG(LOG_INFO, "l = %ld", l );
    uSHELL_LOG(LOG_INFO, "i = %d", i );
    uSHELL_LOG(LOG_INFO, "o = %d", o );

    return 0;
}



///////////////////////////////////////////////////////////////////
//               USER'S SHORTCUTS HANDLERS                       //
///////////////////////////////////////////////////////////////////


/*----------------------------------------------------------------------------*/

void uShellUserHandleShortcut_Slash( const char *pstrArgs )
{
    uSHELL_LOG(LOG_VERBOSE, "[/] shortcut handler | args [%s] called..", pstrArgs);
    uSHELL_LOG(LOG_WARNING, "Not implemented");

} /* uShellUserHandleShortcut_Slash() */


/*----------------------------------------------------------------------------*/

void uShellUserHandleShortcut_Dot( const char *pstrArgs )
{
    uSHELL_LOG(LOG_VERBOSE, "[.] shortcut handler | args [%s] called..", pstrArgs);
    uSHELL_LOG(LOG_WARNING, "Not implemented");

} /* uShellUserHandleShortcut_Dot() */
