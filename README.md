# uShell

A highly configurable and modular micro shell (uShell) system written in C++, designed to serve as a framework for building command-line-based applications on both Linux and Windows platforms.
The architecture ensures a complete separation between the core command-line engine and user-defined commands, which are integrated as plugins.
Additionally, the default plugin supports dynamic loading of external plugins, enabling execution of commands implemented in shared libraries (such as .dll or .so files).


## 🔧 **Core Features**

### 1. **Input from user**
- The command format is straightforward: `COMMAND arg1 arg2 ... argN`, where the arguments must align with the specifications set in the configuration file.
- Any deviation in the number or type of parameters is identified and results in an error.
- Strings that do not contain internal spaces can be used without `""` delimiters; however, if spaces are present, delimiters become required.
- Numeric arguments are checked to ensure they fall within the declared range; if a value exceeds the allowed limits, an error is generated.
- The maximum size of the input buffer is configurable in the global settings file: uShell/sources/ushell_settings/inc/ushell_core_settings.h, e.g.  #define uSHELL_MAX_INPUT_BUF_LEN (128U)
- To prevent buffer overflow, the shell engine automatically blocks further user input once this limit is reached. When the buffer is full, the shell displays the character ] to indicate the limit has been hit and ignores any additional input.
- In normal input mode, the user can only enter characters, delete backward using the Backspace key, or press Enter to execute the command.
- Details about advanced edit mode are provided in the following section.

### 2. **Command Parsing and Execution**
- Supports parsing commands with typed parameters (e.g., integers, floats, strings, booleans).
- Executes functions based on a command-function mapping table.
- Handles command errors with detailed feedback.
- Commands are matched to specific patterns based on the structure of their arguments, enabling automatic validation at runtime. This eliminates the need for any validation logic within the user-implemented functions themselves. As a result, existing functions can be exposed to the shell environment without requiring any modifications
- Validate the number, type, and size of input parameters. For example, if a parameter is expected to be of type uint8_t and the user provides a value greater than 255, an error will be triggered.

### 3. **History Management**
- Maintains a circular buffer of past commands.
- Supports:
  - Navigating history with arrow keys (up and down).
  - Executing previous commands by index.
  - Listing and resetting history.
- Optional file-based history persistence (`.hist_<name>`).
- Each instance, such as when plugins are loaded, generates its own dedicated file.

### 4. **Autocomplete**
- Suggests and completes commands based on partial input.
- Supports cycling through suggestions using arrow keys (left and right).
- Can be toggled on/off via shortcuts.

### 5. **Edit Mode**
- By default, input editing is disabled (except for character deletion using the Backspace key and full-line deletion using the DEL key). However, it can be temporarily enabled for each line by pressing either the TAB or INSERT key
- Enables cursor movement within the input line.
- Supports:
  - Insert/delete characters under or before the cursor.
  - Move cursor to start/end.
  - Use Ctrl+U to delete from the cursor to the start of the line, and Ctrl+K to delete from the cursor to the end of the line.
- Once activated one can navigate through the input string using arrays, delete or insert in place, delete to the b

### 6. **Shortcuts**
- Supports single-character shortcuts (e.g., `#q` to quit).
- Core shortcuts include:
  - `##`: Show the list of the commands
  - `##s`: Show the list of the commands containing the substring `s`
  - `##i`: Show details about the command at index `i` (as reported by `##` or `###` )
  - `###`: Display the list of available commands, along with details about keyboard shortcuts, currently enabled parameter types
  - `#q`: Quit shell
  - `#E` / `#e`: Echo on/off
  - `#A` / `#a`: Autocomplete on/off
  - `#H` / `#h`: History on/off
  - `#l`: List history
  - `#r`: Reset history
  - `#s{}`: Set string delimiter
  - `#k`: Key decoder
- Users can define their own keyboard shortcuts, including both documented and undocumented (hidden) key bindings.

### 7. **Smart Prompt**
- Dynamically updates the shell prompt based on enabled features (e.g., history, autocomplete, edit mode).
- Customizable prompt symbols and formatting.

### 8. **String Handling**
- Supports bordered strings (e.g., `"quoted strings"`).
- Allows setting custom string delimiters.

### 9. **Error Reporting**
- Provides detailed error messages for:
  - Invalid commands
  - Wrong number of arguments
  - Unsupported data types
  - Memory allocation failures

### 10. **Platform Compatibility**
- Designed to work across:
  - Windows (MSVC, MinGW)
  - Linux

### 11. **Build**
- The build process is designed for a Linux environment:
  - Linux: Execute `./linux_build.sh` to start the build.
  - MinGW: Install via `sudo apt-get install mingw-w64` on Linux, then run `./windows_build.sh`.
  - Microsoft Visual Studio: Open the project folder, CMake will automatically detect the configuration, allowing you to initiate the build.


---

## ⚙️ **Configuration via Macros**

The shell is highly configurable through preprocessor macros such as:

- `uSHELL_IMPLEMENTS_HISTORY`
- `uSHELL_IMPLEMENTS_AUTOCOMPLETE`
- `uSHELL_IMPLEMENTS_EDITMODE`
- `uSHELL_IMPLEMENTS_SMART_PROMPT`
- `uSHELL_SUPPORTS_MULTIPLE_INSTANCES`
- `uSHELL_IMPLEMENTS_SAVE_HISTORY`
- `uSHELL_IMPLEMENTS_COMMAND_HELP`
- `uSHELL_IMPLEMENTS_KEY_DECODER`
- `uSHELL_IMPLEMENTS_DISABLE_ECHO`

These allow for tailoring the shell to different environments and resource constraints.


### ⚙️ Configuration Guide for uShell Features

The uShell system is highly configurable through **preprocessor macros**. Below is a guide to enable or disable features by defining or undefining these macros in your build configuration or header files.

| Feature | Macro | Description | Default |
|--------|-------|-------------|---------|
| **Colored items** | `uSHELL_SUPPORTS_COLORS` | Enable colored output. | `1` |
| **Multiple Instances** | `uSHELL_SUPPORTS_MULTIPLE_INSTANCES` | Allows multiple shell instances to run concurrently. | `1` |
| **Command History** | `uSHELL_IMPLEMENTS_HISTORY` | Enables command history using a circular buffer. | `1` |
| **Save History to File** | `uSHELL_IMPLEMENTS_SAVE_HISTORY` | Persists history to a file (e.g., `.hist_<name>`). | `1` |
| **Autocomplete** | `uSHELL_IMPLEMENTS_AUTOCOMPLETE` | Suggests and completes commands based on input. | `1` |
| **Edit Mode** | `uSHELL_IMPLEMENTS_EDITMODE` | Enables cursor movement and in-line editing. | `1` |
| **Smart Prompt** | `uSHELL_IMPLEMENTS_SMART_PROMPT` | Dynamically updates prompt based on active features. | `1` |
| **Command Help** | `uSHELL_IMPLEMENTS_COMMAND_HELP` | Provides detailed help for commands and parameters. | `1` |
| **Key Decoder** | `uSHELL_IMPLEMENTS_KEY_DECODER` | Displays key codes for debugging terminal input. | `1` |
| **Echo Control** | `uSHELL_IMPLEMENTS_DISABLE_ECHO` | Allows toggling input echo on/off. | `1` |
| **String Support** | `uSHELL_IMPLEMENTS_STRINGS` | Enables string parameters in commands. | `1` |
| **Spaced Strings** | `uSHELL_SUPPORTS_SPACED_STRINGS` | Allows strings with spaces using delimiters. | `1` |
| **Shell Exit** | `uSHELL_IMPLEMENTS_SHELL_EXIT` | Enables `#q` shortcut to exit the shell. | `1` |
| **Confirm Requests** | `uSHELL_IMPLEMENTS_CONFIRM_REQUEST` | Prompts for confirmation before critical actions. | `1` |
| **Script Mode** | `uSHELL_SCRIPT_MODE` | Enables script-friendly output formatting. | `0` |

These macros can be customized in the file: uShell/sources/ushell_settings/inc/ushell_core_settings.h

---

### ⚙️ Extendability

Users can easily and flexibly add new commands (functions) to the shell.
If the desired parameter pattern already exists, adding a new function is as simple as copying and pasting a single line.
If the pattern does not exist, it typically requires copying and slightly adapting just three lines of code.
Thanks to the clear separation between the uShell core and user-defined code, users can modify or extend their own functionality without impacting the core.
New functions can be added in the file: uShell/sources/ushell_user/ushell_user_root/inc/ushell_root_commands.cfg under the corresponding parameter pattern, if it already exists.
If the required parameter pattern does not already exist, the user can simply copy and adapt one of the existing patterns to fit their specific needs.

#### Example

Existing pattern

    /*=====================================================================================================*/
    /*                                          l,i,o                                                      */
    /*=====================================================================================================*/
    uSHELL_COMMAND_PARAMS_PATTERN(lio)
    #ifndef lio_params
    #define lio_params                                                                   num64_t,num32_t,bool
    #endif
    /*-----------------------------------------------------------------------------------------------------*/
    uSHELL_COMMAND(dump,                                                                                 lio, "hexdump memory|\taddress - dump address\n\tlength - size of dump\n\tflag - 0/1 (no)show address")


To define a new parameter pattern and add a new function under it, the user can simply copy and slightly adapt an existing pattern definition along with its associated function entry. This process typically involves:

- Copying an existing pattern block from the configuration file.
- Modifying the pattern to match the new command’s expected argument structure.
- Adding the new function under this pattern, following the same format as existing entries.
- This approach keeps the process quick and consistent, requiring minimal effort to extend the shell’s capabilities.

The following snippet illustrates the structure of a new custom parameter pattern:

    /*=====================================================================================================*/
    /*                                          l,i,i,o,s                                                  */
    /*=====================================================================================================*/
    uSHELL_COMMAND_PARAMS_PATTERN(liios)
    #ifndef liios_params
    #define liios_params                                                  num64_t,num32_t,num32_t,bool,str_t*
    #endif
    /*-----------------------------------------------------------------------------------------------------*/
    uSHELL_COMMAND(test_new,                                                                           liios, "Some description with the pattern as above")

Additionally, the new parameter pattern must be registered in the parameter dispatcher. This involves modifying just one more line—the final step in the process. Once this line is added, the new pattern becomes fully integrated and ready to handle commands associated with it.

This change has to be done in the file:  \uShell\sources\ushell_user\ushell_user_root\src\ushell_root_interface.cpp

Existing implementation

    static int uShellExecuteCommand( const command_s *psCmd )
    {
        /* void:v, (byte)u8:b:vb, (word)u16:w:vw, (int)u32:i:vi, (long)u64:l:vl, float:f:vf, string:s:vs, bool:o:vo */
        switch(g_vsFuncDefExArray[psCmd->iFctIndex].eParamType) {
            case v_type          :return g_vsFuncDefExArray[psCmd->iFctIndex].uFctType.v_fct();
            case s_type          :return g_vsFuncDefExArray[psCmd->iFctIndex].uFctType.s_fct(psCmd->vs[0]);
            case lio_type        :return g_vsFuncDefExArray[psCmd->iFctIndex].uFctType.lio_fct(psCmd->vl[0], psCmd->vi[0], psCmd->vo[0]);
            default              :return uSHELL_ERR_PARAMS_PATTERN_NOT_IMPLEM;
        }
    } /* priv_uShellCoreExecuteCommand() */

New implementation (see the last line above the default statement of the switch, the one with liios_type)

    static int uShellExecuteCommand( const command_s *psCmd )
    {
        /* void:v, (byte)u8:b:vb, (word)u16:w:vw, (int)u32:i:vi, (long)u64:l:vl, float:f:vf, string:s:vs, bool:o:vo */
        switch(g_vsFuncDefExArray[psCmd->iFctIndex].eParamType) {
            case v_type          :return g_vsFuncDefExArray[psCmd->iFctIndex].uFctType.v_fct();
            case s_type          :return g_vsFuncDefExArray[psCmd->iFctIndex].uFctType.s_fct(psCmd->vs[0]);
            case lio_type        :return g_vsFuncDefExArray[psCmd->iFctIndex].uFctType.lio_fct(psCmd->vl[0], psCmd->vi[0], psCmd->vo[0]);
            case liios_type      :return g_vsFuncDefExArray[psCmd->iFctIndex].uFctType.lio_fct(psCmd->vl[0], psCmd->vi[0], psCmd->vi[1], psCmd->vo[0], psCmd->vs[0]);
            default              :return uSHELL_ERR_PARAMS_PATTERN_NOT_IMPLEM;
        }
    } /* priv_uShellCoreExecuteCommand() */

Since we added a second parameter of type i (num32_t), it is accessible at index 1 in the integer parameter array vi\[\]. Additionally, because we introduced a string parameter (and it's the first string), it is accessible at index 0 in the string parameter array vs\[\].
For reference, the names and structures of the parameter arrays are defined in: uShell/sources/ushell_core/ushell_core_config/inc/ushell_core_datatypes.h

Note: The maximum number of parameters allowed for each type—and thus the size of the corresponding parameter arrays—is configured in: uShell/sources/ushell_settings/inc/ushell_core_settings.h
If the required number of parameters for a given type exceeds the current configuration, the corresponding value must be updated accordingly to accommodate the new usage.

    #define uSHELL_MAX_PARAMS_NUM64                  (1U)
    #define uSHELL_MAX_PARAMS_NUM32                  (5U)
    #define uSHELL_MAX_PARAMS_NUM16                  (0U)
    #define uSHELL_MAX_PARAMS_NUM8                   (0U)
    #define uSHELL_MAX_PARAMS_FLOAT                  (0U)
    #define uSHELL_MAX_PARAMS_STRING                 (5U)
    #define uSHELL_MAX_PARAMS_BOOLEAN                (1U)

Additionally, the availability of each parameter type must be explicitly enabled in the same configuration file, within the corresponding section.

    #define uSHELL_SUPPORTS_NUMBERS_64BIT            1  /* l (long)   */
    #define uSHELL_SUPPORTS_NUMBERS_32BIT            1  /* i (int)    */
    #define uSHELL_SUPPORTS_NUMBERS_16BIT            0  /* w (word)   */
    #define uSHELL_SUPPORTS_NUMBERS_8BIT             0  /* b (byte)   */
    #define uSHELL_SUPPORTS_NUMBERS_FLOAT            0  /* f (float)  */
    #define uSHELL_SUPPORTS_STRINGS                  1  /* s (string) */
    #define uSHELL_SUPPORTS_BOOLEAN                  1  /* o (bool)   */

