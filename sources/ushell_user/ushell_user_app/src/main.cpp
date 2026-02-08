//#pragma warning(disable : 4668 5039 4710 4711 4820)
#include "ushell_core.h"
#include "ushell_core_terminal.h"

// valgrind --leak-check=yes --track-origins=yes --leak-check=full --show-leak-kinds=all  ./ushell

// g++ -fsanitize=address -g -o ushell *.cpp
// ./ushell

// g++ -fsanitize=undefined -g -o ushell *.cpp
// ./ushell

////////////////////////////////////////////////////////////////////////////////////////
//                                  MAIN                                              //
////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    TerminalRAII terminal;
    static constexpr const char *pstrRootName = "root";

#if (0 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)

    #if (0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)
        Microshell::getShellPtr(uShellPluginEntry(), pstrRootName)->Run();
        
        #if 0  // Optional call patterns
        Microshell *pShell = Microshell::getShellPtr(uShellPluginEntry(), pstrRootName);
        pShell->Execute("vtest");
        pShell->Run();
        #endif
    #else
        Microshell::getShellPtr(uShellPluginEntry(nullptr), pstrRootName)->Run();
    #endif /*(0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)*/

#else /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */

    #if (0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)
        std::shared_ptr<Microshell> pShellPtr = Microshell::getShellSharedPtr(
            uShellPluginEntry(), pstrRootName);
    #else
        std::shared_ptr<Microshell> pShellPtr = Microshell::getShellSharedPtr(
            uShellPluginEntry(nullptr), pstrRootName);
    #endif /*(0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)*/

    if (pShellPtr) {
        pShellPtr->Run();
    } else {
        /* Log error or return error code if shell creation failed */
        return 1;
    }

#endif /* (0 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */

    return 0;

} /* main() */
