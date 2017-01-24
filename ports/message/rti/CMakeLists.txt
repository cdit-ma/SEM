#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME ports_message_rti)
project (${PROJ_NAME})

#Find rtidds library
find_package(RTIDDS REQUIRED)

#Autogen rtiddsgen
execute_process(COMMAND rtiddsgen message.idl -language C++11 -namespace WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/rtitxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rtirxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/rtitxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rtirxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.h
)

set(DDS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/message.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageImpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageImplPlugin.h
)

set(DDS_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/message.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/messageImpl.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/messageImplPlugin.cxx
)

include_directories(${RE_PATH}/include)
include_directories(${DDS_INCLUDE_DIRS})

#Set symbols in dynamic library to be default hidden
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})

#Need this to make sure headers are using the correct function calls.
target_compile_definitions(${PROJ_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${DDS_LIBRARIES})

target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} ports_message)
