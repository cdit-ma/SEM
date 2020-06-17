cmake_minimum_required(VERSION 3.10...3.17)

project(SEM
        VERSION 4.0.0
        DESCRIPTION "Jackson to fill in"
        LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif ()

# Enforce AT LEAST c++11. This project requires c++17 in most places aside from some dependencies (protobuf).
# This needs to be done at top level to ensure that protobuf gets built against c++11.
# Came to light as part of SEM-242, SEM-231
set(CMAKE_CXX_STANDARD 11)

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)
add_subdirectory(re)
#add_subdirectory(medea)
