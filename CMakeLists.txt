cmake_minimum_required(VERSION 3.0)
project(re_top_level_project)

project("re"
        VERSION 3.3.9
        LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Extern dependency builds on mac require at least cpp11
# Sub-projects can (and should) set requirements for newer cpp versions as they need to
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 11)

if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@loader_path;$ORIGIN;@loader_path/../lib/;$ORIGIN/../lib/")
elseif (UNIX)
    set(CMAKE_INSTALL_RPATH "@loader_path;$ORIGIN;@loader_path/../lib/;$ORIGIN/../lib/")
endif ()

# Use CCache where possible
find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

set(RE_VERSION ${PROJECT_VERSION})

option(BUILD_DOCS "Build re developer documentation" ON)
option(BUILD_TEST "Build testing" OFF)
option(BUILD_REGEN_TEST "Build regen testing" OFF)

# Get the RE_PATHRE_PATH
set(RE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
# Set the RE_GEN_PATH
set(REGEN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/re_gen")

set(RE_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LOGAN_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/logan/src")

set(RE_COMMON_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
# Use the local submoduled cmake_modules if we don't have a global one set
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

# Set the binary and library locations
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")

if (MSVC)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/bin")
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/lib")
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/lib")
    #Protobuf says everything is rubbish on windows` https://github.com/google/protobuf/blob/master/cmake/README.md
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /wd4251 /wd4267")
endif ()

# Configure cmakevars.h to have correct string replaced vars
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/cmakevars.h @ONLY)

# Build re_common
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/re_common")

# Build Logan
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/logan")

# Build the source
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

# Build documentation
if (BUILD_DOCS)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/docs")
endif ()

# Build Docker client library
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/extern/DockerClient")

if (BUILD_TEST)
    message(STATUS "Building re tests")
    set(GOOGLE_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/googletest)
    set(GOOGLE_MOCK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/googlemock)

    # Only add googlemock as googlemock builds googletest.
    # Adding googletest subdirectory leads to multiple definition error
    add_subdirectory(${GOOGLE_MOCK_DIR})

    #Build tests
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif (BUILD_TEST)
