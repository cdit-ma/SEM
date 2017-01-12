cmake_minimum_required (VERSION 2.6)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../lib")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJ_NAME re_core)
project (${PROJ_NAME})

set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}../../common")
include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/eventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.h
	)

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

