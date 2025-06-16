#include "ushell_core.h"
#include "ushell_user_plugin_loader.h"
#include "ushell_user_logger.h"

#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
#include <cstring>
#include <memory>

#if defined(_MSC_VER)
    #include <dirent_vs.h>
#else
    #include <dirent.h>
#endif

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif /* _WIN32 */
#endif /*(1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)*/

///////////////////////////////////////////////////////////////////
//            LOCAL DEFINES AND DATA TYPES                       //
///////////////////////////////////////////////////////////////////

#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
    #define PLUGIN_PREFIX               "lib"
    #define SHELL_PLUGINS_PATH          "plugins/"
    #ifdef _WIN32
        #define SHELL_PLUGIN_EXTENSION  "_plugin.dll"
    #else
        #define SHELL_PLUGIN_EXTENSION  "_plugin.so"
    #endif

    #define    SHELL_PLUGIN_ENTRY_POINT_NAME       "uShellPluginEntry"
    #define    SHELL_PLUGIN_EXIT_POINT_NAME        "uShellPluginExit"
#endif /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */

///////////////////////////////////////////////////////////////////
//            PRIVATE INTERFACES DECLARATION                     //
///////////////////////////////////////////////////////////////////

static int privListPlugins (const char *pstrCaption, const char *pstrPath, const char *pstrExtension);

///////////////////////////////////////////////////////////////////
//            EXPORTED VARIABLES DECLARATION                     //
///////////////////////////////////////////////////////////////////

#if (1 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)
    void *pvLocalUserData = nullptr;
#endif /* (1 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA) */

///////////////////////////////////////////////////////////////////
//                 USER COMMANDS IMPLEMENTATION                  //
///////////////////////////////////////////////////////////////////

int vtest ( void )
{
    uSHELL_LOG(LOG_INFO, "vtest called ...");

    return 1;

} /* vtest */


///////////////////////////////////////////////////////////////////
//       PLUGIN RELATED  PUBLIC INTERFACES IMPLEMENTATION        //
///////////////////////////////////////////////////////////////////


#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
/*------------------------------------------------------------
 * list all the available plugins
------------------------------------------------------------*/

#include <cstring>
#include <cstdio>
#include <dirent.h>
#include <unistd.h>


int list(void)
{
#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
    privListPlugins("shell", SHELL_PLUGINS_PATH, SHELL_PLUGIN_EXTENSION);
#endif /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */
    return 0;

} /* list() */


/*------------------------------------------------------------
 * load the plugin
------------------------------------------------------------*/
int pload(char *pstrPluginName)
{
    int iRetVal = 0; // success
    PluginLoaderFunctor<uShellInst_s> loader(PluginPathGenerator(SHELL_PLUGINS_PATH, PLUGIN_PREFIX, SHELL_PLUGIN_EXTENSION),
                                             PluginEntryPointResolver(SHELL_PLUGIN_ENTRY_POINT_NAME, SHELL_PLUGIN_EXIT_POINT_NAME));

    auto handle = loader(pstrPluginName);
    if (!handle.first || !handle.second) {
        uSHELL_LOG(LOG_ERROR, "Failed to load plugin.");
        iRetVal = 0xFF;
    } else {
        uSHELL_LOG(LOG_INFO, "Plugin loaded successfully!");

        auto typedPtr = std::static_pointer_cast<uShellInst_s>(handle.second);
        uShellInst_s* rawPtr = typedPtr.get();

        /* continue execution with the valid shell instance */
        std::shared_ptr<Microshell> pShellPtr = Microshell::getShellSharedPtr(rawPtr, pstrPluginName);

        /* this call is blocking until the shell is released with #q */
        if (nullptr != pShellPtr) {
            pShellPtr->Run();
        }
    }
    return iRetVal;

} /* pload() */

#endif /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */


///////////////////////////////////////////////////////////////////
//               USER SHORTCUTS HANDLERS                         //
///////////////////////////////////////////////////////////////////


#if (1 == uSHELL_IMPLEMENTS_USER_SHORTCUTS)

void uShellUserHandleShortcut_Dot( const char *pstrArgs )
{
    uSHELL_LOG(LOG_WARNING, "[.] registered but not implemented | args[%s] ", pstrArgs);

} /* uShellUserHandleShortcut_Dot() */


/******************************************************************************/
void uShellUserHandleShortcut_Slash( const char *pstrArgs )
{
    uSHELL_LOG(LOG_WARNING, "[/] registered but not implemented | args[%s] ", pstrArgs);

} /* uShellUserHandleShortcut_Slash() */

#endif /*(1 == uSHELL_IMPLEMENTS_USER_SHORTCUTS)*/


///////////////////////////////////////////////////////////////////
//               PRIVATE IMPLEMENTATION                          //
///////////////////////////////////////////////////////////////////

/*------------------------------------------------------------
 * list the available plugins
------------------------------------------------------------*/
static int privListPlugins (const char *pstrCaption, const char *pstrPath, const char *pstrExtension)
{
    #define MAX_WORKBUFFER_SIZE    128U
    char vstrPluginPathName[MAX_WORKBUFFER_SIZE] = {0};
    struct dirent *entry = nullptr;
    DIR *dir = opendir(pstrPath);

    uSHELL_LOG(LOG_INFO, "--- %s plugins ---", pstrCaption);

    if (NULL == dir) {
        uSHELL_LOG(LOG_ERROR, "Failed to open the plugins folder [%s]", pstrPath);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, pstrExtension) != NULL) {
            size_t name_len = strlen(entry->d_name);
            size_t ext_len = strlen(pstrExtension);

            // Ensure safe modification of string
            if (name_len > ext_len) {
                entry->d_name[name_len - ext_len] = '\0';
            }

            int written = snprintf(vstrPluginPathName, MAX_WORKBUFFER_SIZE, "%30s%s | %s", entry->d_name, pstrExtension, entry->d_name + strlen(PLUGIN_PREFIX));
            if (written >= MAX_WORKBUFFER_SIZE) {
                uSHELL_LOG(LOG_WARNING, "Plugin name truncated: [%s]", vstrPluginPathName);
            }

            uSHELL_LOG(LOG_INFO, "%s", vstrPluginPathName);
        }
    }

    closedir(dir);

    return 0;

} /* privListPlugins() */

