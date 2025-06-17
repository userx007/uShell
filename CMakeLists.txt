cmake_minimum_required(VERSION 3.5)
project (ushell)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

set(PLUGINS_LIST test_plugin)

if(MSVC OR MSYS OR MINGW)
    SET(INSTALL_APP_DIR      "bin/windows")
    SET(INSTALL_PLUGIN_DIR   "bin/windows/plugins")
    SET(PLUGIN_INSTALL_TYPE   RUNTIME)
else()
    SET(INSTALL_APP_DIR      "bin/linux")
    SET(INSTALL_PLUGIN_DIR   "bin/linux/plugins")
    SET(PLUGIN_INSTALL_TYPE   LIBRARY)
endif()

if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  message(STATUS "Setting default CMAKE_INSTALL_PREFIX path to ../")
  set(CMAKE_INSTALL_PREFIX "../" CACHE STRING "The path to use for make install" FORCE)
endif()

add_subdirectory(sources)

############################################################
# Install
############################################################


install(TARGETS ushell DESTINATION ${INSTALL_APP_DIR})
install(TARGETS ${PLUGINS_LIST} ${PLUGIN_INSTALL_TYPE} DESTINATION ${INSTALL_PLUGIN_DIR})


