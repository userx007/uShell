#include "ushell_core.h"
#include "ushell_core_utils.h"
#include "ushell_core_printout.h"
#include "ushell_core_settings.h"
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

    #define SHELL_PLUGIN_ENTRY_POINT_NAME       "uShellPluginEntry"
    #define SHELL_PLUGIN_EXIT_POINT_NAME        "uShellPluginExit"
    
    constexpr size_t PLUGIN_NAME_BUFFER_SIZE = 32U;
#endif /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */

///////////////////////////////////////////////////////////////////
//            PRIVATE INTERFACES DECLARATION                     //
///////////////////////////////////////////////////////////////////

static int privListPlugins(const char *pstrCaption, const char *pstrPath, const char *pstrExtension);

///////////////////////////////////////////////////////////////////
//            EXPORTED VARIABLES DECLARATION                     //
///////////////////////////////////////////////////////////////////

#if (1 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)
    void *pvLocalUserData = nullptr;
#endif /* (1 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA) */


///////////////////////////////////////////////////////////////////
//            USER COMMANDS IMPLEMENTATION                       //
///////////////////////////////////////////////////////////////////

int vtest(void)
{
    uSHELL_LOG(LOG_VERBOSE, "--> vtest()" );
    return 0;
}


#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
/*------------------------------------------------------------
 * list all the available plugins
------------------------------------------------------------*/
int list(void)
{
    privListPlugins("shell", SHELL_PLUGINS_PATH, SHELL_PLUGIN_EXTENSION);
    return 0;
} /* list() */


/*------------------------------------------------------------
 * load the plugin
------------------------------------------------------------*/
int pload(char *pstrPluginName)
{
    if (nullptr == pstrPluginName) {
        uSHELL_LOG(LOG_ERROR, "Invalid plugin name (nullptr).");
        return 0xFF;
    }

    PluginLoaderFunctor<uShellInst_s> loader(
        PluginPathGenerator(SHELL_PLUGINS_PATH, PLUGIN_PREFIX, SHELL_PLUGIN_EXTENSION),
        PluginEntryPointResolver(SHELL_PLUGIN_ENTRY_POINT_NAME, SHELL_PLUGIN_EXIT_POINT_NAME)
    );

    auto handle = loader(pstrPluginName);
    
    if (!handle.first || !handle.second) {
        uSHELL_LOG(LOG_ERROR, "Failed to load plugin: %s", pstrPluginName);
        return 0xFF;
    }
    
    uSHELL_LOG(LOG_INFO, "Plugin loaded successfully: %s", pstrPluginName);

    uShellInst_s* pShellInst = static_cast<uShellInst_s*>(handle.second.get());
    
    /* Continue execution with the valid shell instance */
    std::shared_ptr<Microshell> pShellPtr = Microshell::getShellSharedPtr(pShellInst, pstrPluginName);

    /* This call is blocking until the shell is released with #q */
    if (nullptr != pShellPtr) {
        pShellPtr->Run();
        return 0;
    }
    
    uSHELL_LOG(LOG_ERROR, "Failed to create shell instance for plugin: %s", pstrPluginName);
    return 0xFF;

} /* pload() */

#endif /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */


///////////////////////////////////////////////////////////////////
//               USER SHORTCUTS HANDLERS                         //
///////////////////////////////////////////////////////////////////

#if (1 == uSHELL_IMPLEMENTS_USER_SHORTCUTS)

void uShellUserHandleShortcut_Dot( const char *pstrArgs )
{
    uSHELL_LOG(LOG_WARNING, "[.] registered but not implemented | args[%s]", pstrArgs);
} /* uShellUserHandleShortcut_Dot() */


/******************************************************************************/
void uShellUserHandleShortcut_Slash( const char *pstrArgs )
{
    uSHELL_LOG(LOG_WARNING, "[/] registered but not implemented | args[%s]", pstrArgs);
} /* uShellUserHandleShortcut_Slash() */

#endif /*(1 == uSHELL_IMPLEMENTS_USER_SHORTCUTS)*/


///////////////////////////////////////////////////////////////////
//               PRIVATE IMPLEMENTATION                          //
///////////////////////////////////////////////////////////////////

/*------------------------------------------------------------
 * list the available plugins
------------------------------------------------------------*/
#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
static int privListPlugins(const char *pstrCaption, const char *pstrPath, const char *pstrExtension)
{
    if (nullptr == pstrPath || nullptr == pstrExtension || nullptr == pstrCaption) {
        uSHELL_LOG(LOG_ERROR, "Invalid parameters to privListPlugins");
        return 1;
    }

    DIR *dir = opendir(pstrPath);
    if (nullptr == dir) {
        uSHELL_LOG(LOG_ERROR, "Failed to open the plugins folder [%s]", pstrPath);
        return 1;
    }

    uSHELL_LOG(LOG_INFO, "--- %s plugins ---", pstrCaption);

    const size_t ext_len = strlen(pstrExtension);
    const size_t prefix_len = strlen(PLUGIN_PREFIX);
    
    struct dirent *entry = nullptr;
    char vstrPluginPathName[PLUGIN_NAME_BUFFER_SIZE];
    
    while ((entry = readdir(dir)) != nullptr) {
        /* Check if filename ends with the extension */
        const char *ext_pos = strstr(entry->d_name, pstrExtension);
        if (nullptr == ext_pos) {
            continue;
        }
        
        const size_t name_len = strlen(entry->d_name);
        
        /* Verify extension is at the end of the filename */
        if (ext_pos != entry->d_name + name_len - ext_len) {
            continue;
        }

        char plugin_name_without_ext[PLUGIN_NAME_BUFFER_SIZE];
        const size_t copy_len = name_len - ext_len;
        
        if (copy_len >= PLUGIN_NAME_BUFFER_SIZE) {
            uSHELL_LOG(LOG_WARNING, "Plugin name too long: [%s]", entry->d_name);
            continue;
        }
        
        strncpy(plugin_name_without_ext, entry->d_name, copy_len);
        plugin_name_without_ext[copy_len] = '\0';

        /* Extract display name (remove prefix if present) */
        const char *display_name = plugin_name_without_ext;
        if (strncmp(plugin_name_without_ext, PLUGIN_PREFIX, prefix_len) == 0) {
            display_name = plugin_name_without_ext + prefix_len;
        }

        /* Format and display */
        int written = snprintf(vstrPluginPathName, PLUGIN_NAME_BUFFER_SIZE, 
                              "%30s%s | %s", 
                              plugin_name_without_ext, pstrExtension, display_name);
        
        if (written < 0) {
            uSHELL_LOG(LOG_ERROR, "Error formatting plugin name");
            continue;
        }
        
        if (static_cast<size_t>(written) >= PLUGIN_NAME_BUFFER_SIZE) {
            uSHELL_LOG(LOG_WARNING, "Plugin name truncated: [%s]", entry->d_name);
        }

        uSHELL_LOG(LOG_INFO, "%s", vstrPluginPathName);
    }

    closedir(dir);
    return 0;

} /* privListPlugins() */
#endif /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */
