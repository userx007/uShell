#include "ushell_core.h"
#include "ushell_core_utils.h"
#include "ushell_core_printout.h"
#include "ushell_core_settings.h"
#include "ushell_user_plugin_loader.h"
#include "ushell_user_logger.h"

#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
#include <cstring>
#include <memory>
#include <string>

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
    static constexpr const char* PLUGIN_PREFIX = "lib";
    static constexpr const char* SHELL_PLUGINS_PATH = "plugins/";
    
    #ifdef _WIN32
        static constexpr const char* SHELL_PLUGIN_EXTENSION = "_plugin.dll";
    #else
        static constexpr const char* SHELL_PLUGIN_EXTENSION = "_plugin.so";
    #endif

    static constexpr const char* SHELL_PLUGIN_ENTRY_POINT_NAME = "uShellPluginEntry";
    static constexpr const char* SHELL_PLUGIN_EXIT_POINT_NAME = "uShellPluginExit";
    
    /* Buffer sizes */
    static constexpr size_t PLUGIN_NAME_BUFFER_SIZE = 128U;
    static constexpr int PLUGIN_NAME_DISPLAY_WIDTH = 30;
    
    /* Error codes - standardized */
    enum PluginErrorCode {
        PLUGIN_SUCCESS = 0,
        PLUGIN_ERROR_INVALID_PARAM = 1,
        PLUGIN_ERROR_LOAD_FAILED = 2,
        PLUGIN_ERROR_INSTANCE_FAILED = 3,
        PLUGIN_ERROR_DIR_OPEN_FAILED = 4,
        PLUGIN_ERROR_BUFFER_OVERFLOW = 5
    };
    
    /* RAII wrapper for DIR* */
    class DirHandle {
    private:
        DIR* dir_;
        
        /* Non-copyable */
        DirHandle(const DirHandle&) = delete;
        DirHandle& operator=(const DirHandle&) = delete;
        
    public:
        explicit DirHandle(const char* path) : dir_(opendir(path)) {}
        
        ~DirHandle() { 
            if (dir_) {
                closedir(dir_); 
            }
        }
        
        /* Move support */
        DirHandle(DirHandle&& other) noexcept : dir_(other.dir_) {
            other.dir_ = nullptr;
        }
        
        DirHandle& operator=(DirHandle&& other) noexcept {
            if (this != &other) {
                if (dir_) {
                    closedir(dir_);
                }
                dir_ = other.dir_;
                other.dir_ = nullptr;
            }
            return *this;
        }
        
        DIR* get() const { return dir_; }
        explicit operator bool() const { return dir_ != nullptr; }
    };
#endif /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */

///////////////////////////////////////////////////////////////////
//            PRIVATE INTERFACES DECLARATION                     //
///////////////////////////////////////////////////////////////////

#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
static int privListPlugins(const char *pstrCaption, const char *pstrPath, const char *pstrExtension);
static bool privExtractPluginDisplayName(const char *filename, const char *extension, 
                                         const char *prefix, std::string &displayName);
#endif

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
    uSHELL_LOG(LOG_VERBOSE, "--> vtest()");
    return PLUGIN_SUCCESS;
}


#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
/*------------------------------------------------------------
 * list all the available plugins
------------------------------------------------------------*/
int list(void)
{
    const int result = privListPlugins("shell", SHELL_PLUGINS_PATH, SHELL_PLUGIN_EXTENSION);
    return (result == PLUGIN_SUCCESS) ? 0 : result;
} /* list() */


/*------------------------------------------------------------
 * load the plugin
 * : Improved error handling and const correctness
------------------------------------------------------------*/
int pload(char *pstrPluginName)
{
    /* Validate input */
    if (nullptr == pstrPluginName || '\0' == *pstrPluginName) {
        uSHELL_LOG(LOG_ERROR, "Invalid plugin name (nullptr or empty).");
        return PLUGIN_ERROR_INVALID_PARAM;
    }

    /* Create plugin loader */
    PluginLoaderFunctor<uShellInst_s> loader(
        PluginPathGenerator(SHELL_PLUGINS_PATH, PLUGIN_PREFIX, SHELL_PLUGIN_EXTENSION),
        PluginEntryPointResolver(SHELL_PLUGIN_ENTRY_POINT_NAME, SHELL_PLUGIN_EXIT_POINT_NAME)
    );

    /* Load the plugin */
    auto handle = loader(pstrPluginName);
    
    if (!handle.first || !handle.second) {
        uSHELL_LOG(LOG_ERROR, "Failed to load plugin: %s", pstrPluginName);
        return PLUGIN_ERROR_LOAD_FAILED;
    }
    
    uSHELL_LOG(LOG_INFO, "Plugin loaded successfully: %s", pstrPluginName);

    /* Get shell instance from plugin */
    uShellInst_s* pShellInst = static_cast<uShellInst_s*>(handle.second.get());
    if (nullptr == pShellInst) {
        uSHELL_LOG(LOG_ERROR, "Plugin returned null instance: %s", pstrPluginName);
        return PLUGIN_ERROR_INSTANCE_FAILED;
    }
    
    /* Create microshell wrapper */
    std::shared_ptr<Microshell> pShellPtr = Microshell::getShellSharedPtr(pShellInst, pstrPluginName);
    if (!pShellPtr) {
        uSHELL_LOG(LOG_ERROR, "Failed to create shell instance for plugin: %s", pstrPluginName);
        return PLUGIN_ERROR_INSTANCE_FAILED;
    }

    /* Run the shell (blocking until exit) */
    pShellPtr->Run();
    
    return PLUGIN_SUCCESS;

} /* pload() */


/*------------------------------------------------------------
 * Extract plugin display name from filename
------------------------------------------------------------*/
static bool privExtractPluginDisplayName(const char *filename, const char *extension, 
                                         const char *prefix, std::string &displayName)
{
    if (!filename || !extension || !prefix) {
        return false;
    }
    
    /* Find extension in filename */
    const char *ext_pos = strstr(filename, extension);
    if (!ext_pos) {
        return false;
    }
    
    const size_t name_len = strlen(filename);
    const size_t ext_len = strlen(extension);
    
    /* Verify extension is at the end */
    if (ext_pos != filename + name_len - ext_len) {
        return false;
    }
    
    /* Extract name without extension */
    const size_t base_len = name_len - ext_len;
    std::string baseName(filename, base_len);
    
    /* Remove prefix if present */
    const size_t prefix_len = strlen(prefix);
    if (baseName.length() > prefix_len && baseName.substr(0, prefix_len) == prefix) {
        displayName = baseName.substr(prefix_len);
    } else {
        displayName = baseName;
    }
    
    return true;
}


/*------------------------------------------------------------
 * list the available plugins
------------------------------------------------------------*/
static int privListPlugins(const char *pstrCaption, const char *pstrPath, const char *pstrExtension)
{
    /* Validate parameters */
    if (!pstrPath || !pstrExtension || !pstrCaption) {
        uSHELL_LOG(LOG_ERROR, "Invalid parameters to privListPlugins");
        return PLUGIN_ERROR_INVALID_PARAM;
    }

    /* Open directory with RAII */
    DirHandle dir(pstrPath);
    if (!dir) {
        uSHELL_LOG(LOG_ERROR, "Failed to open the plugins folder [%s]", pstrPath);
        return PLUGIN_ERROR_DIR_OPEN_FAILED;
    }

    uSHELL_LOG(LOG_INFO, "--- %s plugins ---", pstrCaption);

    /* Cache string lengths outside loop */
    const size_t ext_len = strlen(pstrExtension);
    const size_t prefix_len = strlen(PLUGIN_PREFIX);
    
    struct dirent *entry = nullptr;
    int plugin_count = 0;
    
    while ((entry = readdir(dir.get())) != nullptr) {
        /* Extract display name */
        std::string displayName;
        if (!privExtractPluginDisplayName(entry->d_name, pstrExtension, PLUGIN_PREFIX, displayName)) {
            continue;
        }
        
        /* Create full name (base + extension) */
        const size_t name_len = strlen(entry->d_name);
        const size_t base_len = name_len - ext_len;
        std::string fullName(entry->d_name, base_len);
        
        /* Format output line using snprintf for safety */
        char formatted_line[PLUGIN_NAME_BUFFER_SIZE];
        const int written = snprintf(formatted_line, PLUGIN_NAME_BUFFER_SIZE, 
                                     "%*s%s | %s", 
                                     PLUGIN_NAME_DISPLAY_WIDTH,
                                     fullName.c_str(),
                                     pstrExtension, 
                                     displayName.c_str());
        
        /* Check for errors first, then truncation */
        if (written < 0) {
            uSHELL_LOG(LOG_ERROR, "Error formatting plugin name: %s", entry->d_name);
            continue;
        }
        
        if (static_cast<size_t>(written) >= PLUGIN_NAME_BUFFER_SIZE) {
            uSHELL_LOG(LOG_WARNING, "Plugin name truncated: [%s]", entry->d_name);
            /* Continue anyway - truncated output is better than no output */
        }

        uSHELL_LOG(LOG_INFO, "%s", formatted_line);
        plugin_count++;
    }

    if (plugin_count == 0) {
        uSHELL_LOG(LOG_INFO, "No plugins found in %s", pstrPath);
    }

    return PLUGIN_SUCCESS;

} /* privListPlugins() */

#endif /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */


///////////////////////////////////////////////////////////////////
//               USER SHORTCUTS HANDLERS                         //
///////////////////////////////////////////////////////////////////

#if (1 == uSHELL_IMPLEMENTS_USER_SHORTCUTS)

void uShellUserHandleShortcut_Dot(const char *pstrArgs)
{
    uSHELL_LOG(LOG_WARNING, "[.] registered but not implemented | args[%s]", 
               pstrArgs ? pstrArgs : "(null)");
} /* uShellUserHandleShortcut_Dot() */


/******************************************************************************/
void uShellUserHandleShortcut_Slash(const char *pstrArgs)
{
    uSHELL_LOG(LOG_WARNING, "[/] registered but not implemented | args[%s]", 
               pstrArgs ? pstrArgs : "(null)");
} /* uShellUserHandleShortcut_Slash() */

#endif /*(1 == uSHELL_IMPLEMENTS_USER_SHORTCUTS)*/
