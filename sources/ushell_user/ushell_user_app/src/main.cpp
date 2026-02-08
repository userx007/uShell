//#pragma warning(disable : 4668 5039 4710 4711 4820)
#include "ushell_core.h"
#include "ushell_core_terminal.h"

#include <cstdlib>

// valgrind --leak-check=yes --track-origins=yes --leak-check=full --show-leak-kinds=all  ./ushell

// g++ -fsanitize=address -g -o ushell *.cpp
// ./ushell

// g++ -fsanitize=undefined -g -o ushell *.cpp
// ./ushell

////////////////////////////////////////////////////////////////////////////////////////
//                            CONSTANTS AND ERROR CODES                               //
////////////////////////////////////////////////////////////////////////////////////////

/* Standardized exit codes */
enum ExitCode {
    EXIT_SUCCESS_CODE = 0,
    EXIT_PLUGIN_INIT_FAILED = 1,
    EXIT_SHELL_CREATION_FAILED = 2,
    EXIT_TERMINAL_INIT_FAILED = 3
};

/* Shell configuration */
static constexpr const char* ROOT_SHELL_NAME = "root";

////////////////////////////////////////////////////////////////////////////////////////
//                            HELPER FUNCTIONS                                        //
////////////////////////////////////////////////////////////////////////////////////////

#if (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
/**
 * @brief Initialize and run shell with multiple instance support
 * @param pShellInst Pointer to shell instance configuration
 * @param pstrShellName Name for the shell prompt
 * @return Exit code
 */
static int runShellMultiInstance(uShellInst_s *pShellInst, const char *pstrShellName)
{
    if (!pShellInst) {
        return EXIT_PLUGIN_INIT_FAILED;
    }
    
    std::shared_ptr<Microshell> pShellPtr = Microshell::getShellSharedPtr(pShellInst, pstrShellName);
    
    if (!pShellPtr) {
        return EXIT_SHELL_CREATION_FAILED;
    }
    
    pShellPtr->Run();
    return EXIT_SUCCESS_CODE;
}
#else
/**
 * @brief Initialize and run shell with single instance support
 * @param pShellInst Pointer to shell instance configuration
 * @param pstrShellName Name for the shell prompt
 * @return Exit code
 */
static int runShellSingleInstance(uShellInst_s *pShellInst, const char *pstrShellName)
{
    if (!pShellInst) {
        return EXIT_PLUGIN_INIT_FAILED;
    }
    
    Microshell *pShell = Microshell::getShellPtr(pShellInst, pstrShellName);
    
    if (!pShell) {
        return EXIT_SHELL_CREATION_FAILED;
    }
    
    pShell->Run();
    return EXIT_SUCCESS_CODE;
}
#endif /* uSHELL_SUPPORTS_MULTIPLE_INSTANCES */


////////////////////////////////////////////////////////////////////////////////////////
//                                  MAIN                                              //
////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    int exitCode = EXIT_SUCCESS_CODE;
    
    /* Initialize terminal with RAII */
    TerminalRAII terminal;

#if (0 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)
    /* Single instance mode */
    
    #if (0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)
        uShellInst_s *pShellInst = uShellPluginEntry();
    #else
        uShellInst_s *pShellInst = uShellPluginEntry(nullptr);
    #endif
    
    exitCode = runShellSingleInstance(pShellInst, ROOT_SHELL_NAME);
    
    #if 0  /* Optional: Execute command before running interactive shell */
    if (exitCode == EXIT_SUCCESS_CODE) {
        Microshell *pShell = Microshell::getShellPtr(pShellInst, ROOT_SHELL_NAME);
        if (pShell && pShell->Execute("vtest")) {
            pShell->Run();
        }
    }
    #endif

#else /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */
    /* Multiple instance mode */
    
    #if (0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)
        uShellInst_s *pShellInst = uShellPluginEntry();
    #else
        uShellInst_s *pShellInst = uShellPluginEntry(nullptr);
    #endif
    
    exitCode = runShellMultiInstance(pShellInst, ROOT_SHELL_NAME);

#endif /* (0 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */

    return exitCode;

} /* main() */
