# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (ushell)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND flags "-Os" "-Wall")
if(NOT WIN32)
  list(APPEND flags "-Wextra" "-Wpedantic")
endif()


set(SOURCES   src/main.cpp
)

add_executable( ${PROJECT_NAME} ${SOURCES})

target_compile_options( ${PROJECT_NAME}
  PRIVATE
    ${flags}
)

target_link_libraries( ${PROJECT_NAME}
    ushell_core
    ushell_core_terminal
    ushell_core_utils
    ushell_user_root
    ushell_user_logger
)

