#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME ports_message_ospl)
project (${PROJ_NAME})

#Find ospl library
find_package(OpenSplice REQUIRED)

#Autogen idl generation
execute_process(COMMAND idlpp message.idl -l isocpp -S WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenSplice REQUIRED)


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

include_directories(${RE_PATH}/includes)
include_directories(${OpenSplice_INCLUDE_DIRS})

#Set symbols in dynamic library to be default hidden
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${OpenSplice_LIBRARIES})

target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} ports_message)