cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set our module path
set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CDITMA_RUNTIME_ROOT}/cmake/modules")

set(PROJ_NAME ospl_library)
project (${PROJ_NAME})

include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

find_package(OpenSplice REQUIRED)
include_directories(${PROJ_NAME} ${OpenSplice_INCLUDE_DIRS})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ospltxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/osplrxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.cpp
    #include the message type
    #${CMAKE_CURRENT_SOURCE_DIR}/../message.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ospltxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/osplrxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.h
    #include the message type
    #${CMAKE_CURRENT_SOURCE_DIR}/../message.h
)

set(DDS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageSplDcps.h
    ${CMAKE_CURRENT_SOURCE_DIR}/message_DCPS.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.cpp
)

set(DDS_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageSplDcps.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps_impl.cpp
)

#Set symbols in dynamic library to be default hidden
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})

target_link_libraries(${PROJ_NAME} ${OpenSplice_LIBRARIES})