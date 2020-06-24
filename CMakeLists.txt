cmake_minimum_required(VERSION 3.10...3.17)

project(SEM
        VERSION 4.0.0
        DESCRIPTION "TODO: Fill in description"
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/cdit-ma/SEM")

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif ()

# Enforce AT LEAST c++11. This project requires c++17 in most places aside from some dependencies (protobuf).
# This needs to be done at top level to ensure that protobuf gets built against c++11.
# Came to light as part of SEM-242, SEM-231
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Global build options
option(BUILD_DOCS "Build re developer documentation" OFF)
option(BUILD_TEST "Build testing" OFF)
option(BUILD_REGEN_TEST "Build regen testing" OFF)
option(BUILD_MEDEA "Build MEDEA" ON)

add_subdirectory(src)
add_subdirectory(re)

if (BUILD_GUI)
    add_subdirectory(medea)
endif ()
