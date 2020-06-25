project("re"
        VERSION ${PROJECT_VERSION}
        LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@loader_path;$ORIGIN;@loader_path/../lib/;$ORIGIN/../lib/")
elseif (UNIX)
    set(CMAKE_INSTALL_RPATH "@loader_path;$ORIGIN;@loader_path/../lib/;$ORIGIN/../lib/")
endif ()

set(RE_VERSION ${PROJECT_VERSION})

# Get the RE_PATH
set(RE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
# Set the RE_GEN_PATH
set(REGEN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/re_gen")

set(RE_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LOGAN_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/logan/src")

set(RE_COMMON_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
# Use the local submoduled cmake_modules if we don't have a global one set
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

# Set the binary and library locations
set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
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

# Build CUDA cpp wrapper library
include(CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
    add_subdirectory(extern/cuda-api-wrappers)
endif()

if (BUILD_TEST)
    set(GOOGLE_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/googletest)
    set(GOOGLE_MOCK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/googlemock)

    # Only add googlemock as googlemock builds googletest.
    # Adding googletest subdirectory leads to multiple definition error
    add_subdirectory(${GOOGLE_MOCK_DIR})
endif (BUILD_TEST)

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
    #Build tests
    message(STATUS "Building re tests")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif (BUILD_TEST)
