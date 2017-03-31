cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#get the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{CMAKE_MODULE_PATH}/cmake/modules")

set(PROJ_NAME zmq_protowriter)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(ZMQ REQUIRED)
include_directories(${ZMQ_INCLUDE_DIRS})

#Link against boost dynamically
add_definitions(-DBOOST_ALL_DYN_LINK)
#Need boost > 1.30.0 system and filesystem for crossplatform temp file creation.
find_package(Boost 1.30.0 COMPONENTS filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protowriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cachedprotowriter.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protowriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cachedprotowriter.h
    )



if (MSVC)
    #Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
    #Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

#Link against our required libraries
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_SYSTEM_LIBRARY})
target_link_libraries(${PROJ_NAME} ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(${PROJ_NAME} zmq_monitor)