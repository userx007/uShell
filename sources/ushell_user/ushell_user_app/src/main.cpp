#include "ushell_core.h"
#include "ushell_core_terminal.h"


// valgrind --leak-check=yes --track-origins=yes --leak-check=full --show-leak-kinds=all  ./ushell

////////////////////////////////////////////////////////////////////////////////////////
//                                  MAIN                                              //
////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    TerminalRAII terminal;
    const char *pstrRootName = "root";

#if (0 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES)

    #if (0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)
        Microshell::getShellPtr(pluginEntry(), pstrRootName)->Run();
    #else
        Microshell::getShellPtr(pluginEntry(nullptr), pstrRootName)->Run();
    #endif /*(0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)*/

#else /* (1 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */

    #if (0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)
        std::shared_ptr<Microshell> pShellPtr = Microshell::getShellSharedPtr(pluginEntry(), pstrRootName);
    #else
        std::shared_ptr<Microshell> pShellPtr = Microshell::getShellSharedPtr(pluginEntry(nullptr), pstrRootName);
    #endif /*(0 == uSHELL_SUPPORTS_EXTERNAL_USER_DATA)*/

    if (nullptr != pShellPtr) {
        pShellPtr->Run();
    }

#endif /* (0 == uSHELL_SUPPORTS_MULTIPLE_INSTANCES) */

    return  0;

} /* main() */
