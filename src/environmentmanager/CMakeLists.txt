cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{CMAKE_MODULE_PATH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

#Include the parent directory
include_directories("../")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/environmentmessage")

set(ENVIRONMENT_MANAGER environment_manager)
set(ENVIRONMENT_MANAGER_LIB "${ENVIRONMENT_MANAGER}_lib")

set(PROJ_NAME ${ENVIRONMENT_MANAGER})
project(${PROJ_NAME})

#Find ZeroMQ
find_package(ZMQ REQUIRED)
#Find Protobuf
find_package(Protobuf REQUIRED)

#Link against boost dynamically
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/broadcaster.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/broadcaster.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.h
    ${CMAKE_CURRENT_SOURCE_DIR}/environment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.h
    )

    
add_library(${ENVIRONMENT_MANAGER_LIB} ${SOURCE} ${HEADERS})

add_executable(${ENVIRONMENT_MANAGER} ${CMAKE_CURRENT_SOURCE_DIR}/environmentmanager.cpp)

target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})

target_compile_definitions(${ENVIRONMENT_MANAGER} PRIVATE -DBOOST_ALL_DYN_LINK)

if (MSVC)
#Windows requires protobuf in DLLs
    target_compile_definitions(${ENVIRONMENT_MANAGER} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
    target_link_libraries(${ENVIRONMENT_MANAGER} pthread)
    target_link_libraries(${ENVIRONMENT_MANAGER} dl)
endif(MSVC)

#Link executable against all libraries required
target_link_libraries(${ENVIRONMENT_MANAGER} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} ${ZMQ_LIBRARIES})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} ${PROTOBUF_LIBRARIES})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} environment_proto_message)
target_link_libraries(${ENVIRONMENT_MANAGER} ${ENVIRONMENT_MANAGER_LIB})

