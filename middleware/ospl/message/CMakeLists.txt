#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME ospl_message)
project (${PROJ_NAME})

#Find ospl library
find_package(OpenSplice REQUIRED)

#Autogen idl generation
#execute_process(COMMAND idlpp message.idl -l isocpp -S WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenSplice REQUIRED)

OSPL_GENERATE_CPP(DDS_SOURCE DDS_HEADERS message.idl)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/tx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/tx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/convert.h
)


message(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${RE_PATH}/includes)
include_directories(${OpenSplice_INCLUDE_DIRS})

#Set symbols in dynamic library to be default hidden
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${OpenSplice_LIBRARIES})

#target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} datatypes_message)