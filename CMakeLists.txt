cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Required for dynamic linking magic
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(RE_VERSION "1.5.0")

option(BUILD_TEST "Build testing" OFF)


#Get the RE_PATH
set(RE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
#Set the RE_GEN_PATH
set(REGEN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/re_gen")

set(RE_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RE_COMMON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/re_common")
#Use the local submoduled cmake_modules if we don't have a global one set
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

#Set the binary and library locations

#set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
# Uncomment for standard stuff
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")

if(MSVC)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/bin")
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/lib")
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/lib")
    #Protobuf says everything is rubbish on windows` https://github.com/google/protobuf/blob/master/cmake/README.md
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /wd4251 /wd4267")
endif()

#Build the source
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

if(BUILD_TEST)
	message(STATUS "Building re tests")
	#Build the test
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif(BUILD_TEST)
