cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#get the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{CMAKE_MODULE_PATH}")

set(PROJ_NAME modelprotohandler)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/modelprotohandler.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/modelprotohandler.h
    )

if (MSVC)
    #Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
    #Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})

#Link against our required libraries
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} re_common_proto_modelevent)
