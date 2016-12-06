cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(PROJ_NAME ospl_library)
project (${PROJ_NAME})

find_package(OpenSplice REQUIRED)
include_directories(${PROJ_NAME} ${OpenSplice_INCLUDE_DIRS})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/osplrxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ospltxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/osplhelper.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/../message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../basemessage.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/osplrxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ospltxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/osplhelper.h

)

set(DDS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/message_DCPS.hpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageSplDcps.h
)

set(DDS_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageSplDcps.cpp
)


add_library(${PROJ_NAME} ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})

target_link_libraries(${PROJ_NAME} ${OpenSplice_LIBRARIES})
