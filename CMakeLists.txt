cmake_minimum_required(VERSION 3.5)
project (ushell)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

set(PLUGINS_LIST math_plugin)

SET(INSTALL_WIN_APP_DIR      "bin/windows")
SET(INSTALL_WIN_PLUGIN_DIR   "bin/windows/plugins")
SET(INSTALL_LINUX_APP_DIR    "bin/linux")
SET(INSTALL_LINUX_PLUGIN_DIR "bin/linux/plugins")


if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  message(STATUS "Setting default CMAKE_INSTALL_PREFIX path to ../")
  set(CMAKE_INSTALL_PREFIX "../" CACHE STRING "The path to use for make install" FORCE)
endif()

add_subdirectory(sources)

############################################################
# Install
############################################################


if(MSVC OR MSYS OR MINGW)

    # --- Binaries ---
    install ( TARGETS ushell
        DESTINATION ${INSTALL_WIN_APP_DIR}
    )

    # --- Plugins ---
    install ( TARGETS ${PLUGINS_LIST}
        RUNTIME DESTINATION ${INSTALL_WIN_PLUGIN_DIR}
    )

else()

    # --- Binaries ---
    install ( TARGETS ushell
        DESTINATION ${INSTALL_LINUX_APP_DIR}
    )

    # --- Plugins ---
    install ( TARGETS ${PLUGINS_LIST}
        LIBRARY DESTINATION ${INSTALL_LINUX_PLUGIN_DIR}
    )

endif()
