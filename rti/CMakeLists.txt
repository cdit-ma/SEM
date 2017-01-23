cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set our module path
set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CDITMA_RUNTIME_ROOT}/cmake/modules")

set(PROJ_NAME rti_library)
project (${PROJ_NAME})

include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

find_package(RTIDDS REQUIRED)

execute_process(COMMAND rtiddsgen message.idl -language C++11 -namespace WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${PROJ_NAME} ${DDS_INCLUDE_DIRS})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/rtitxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rtirxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/../message.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/rtitxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rtirxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.h
    #${CMAKE_CURRENT_SOURCE_DIR}/../message.h
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

#Set symbols in dynamic library to be default hidden
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})

#Need this to make sure headers are using the correct function calls.
target_compile_definitions(${PROJ_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)

target_link_libraries(${PROJ_NAME} ${DDS_LIBRARIES})
