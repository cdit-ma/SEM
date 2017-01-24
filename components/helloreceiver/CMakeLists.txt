cmake_minimum_required (VERSION 2.6)

#set(CMAKE_MACOSX_RPATH 1)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#set(CMAKE_INSTALL_RPATH "@executable_path;@loader_path;$ORIGIN;")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set our module path
set(CDITMA_RUNTIME_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../common/)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CDITMA_RUNTIME_ROOT}/cmake/modules")

include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

set(PROJ_NAME components_helloreceiver)
project (${PROJ_NAME})

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${CDITMA_RUNTIME_ROOT}/lib")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/helloreceiverimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/helloreceiverint.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/helloreceiverimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/helloreceiverint.h
	)

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ports_message)