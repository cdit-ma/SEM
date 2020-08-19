cmake_minimum_required(VERSION 3.11...3.17)

project(SEM
        VERSION 4.0.0
        DESCRIPTION "The System Execution Modelling project maintained by the Centre for Distributed and Intelligent Technologies"
        LANGUAGES CXX)

set(HOMEPAGE_URL "https://github.com/cdit-ma/SEM")

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif ()

if (MSVC)
    set(CMAKE_CXX_COMPILER_LAUNCHER sccache)
    set(CMAKE_C_COMPILER_LAUNCHER sccache)
    # Remove instances of Zi in MSVC invocation flags to let sccache work
    #  (sscache currently runs into errors due to .pdb files being created for the whole
    #  directory, meaning that multiple processes lock each other out from file accesses)
    string(REGEX REPLACE "/Z[iI7]" ""
            CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Z7")
endif (MSVC)


# Enforce AT LEAST c++11. This project requires c++17 in most places aside from some dependencies (protobuf).
# This needs to be done at top level to ensure that protobuf gets built against c++11.
# Came to light as part of SEM-242, SEM-231
set(CMAKE_CXX_STANDARD 17)
# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Set library and binary output directories
# TODO: Investigate generator patterns for release/debug builds.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


# Global build options
option(BUILD_DOCS "Build re developer documentation" OFF)
option(BUILD_TEST "Build testing" OFF)
option(BUILD_REGEN_TEST "Build regen testing" OFF)
option(BUILD_MEDEA "Build MEDEA" ON)
option(NO_SIGAR "Disable Hypiric SIGAR dependency" OFF)
option(SEM_DEPS_BASE_DIR "")
if (SEM_DEPS_BASE_DIR)
    set(FETCHCONTENT_BASE_DIR ${SEM_DEPS_BASE_DIR}/${CMAKE_BUILD_TYPE} CACHE PATH "")
endif ()

add_subdirectory(extern)


add_subdirectory(src)
add_subdirectory(re)
set(REGEN_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/re/re_gen")

if (BUILD_MEDEA)
    add_subdirectory(medea)
endif ()
