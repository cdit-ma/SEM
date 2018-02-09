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

set(PROJ_NAME ${ENVIRONMENT_MANAGER})
project(${PROJ_NAME})

#Find ZeroMQ
find_package(ZMQ REQUIRED)
#Find Protobuf
find_package(Protobuf REQUIRED)

#Link against boost dynamically
add_definitions(-DBOOST_ALL_DYN_LINK)
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentmanager.cpp
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

add_executable(${ENVIRONMENT_MANAGER} ${SOURCE} ${HEADERS})

set(TEST_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)

add_executable(reg_tester ${TEST_SRC})

target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})

target_include_directories(reg_tester PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(reg_tester PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(reg_tester PRIVATE ${PROTOBUF_INCLUDE_DIRS})

if (MSVC)
    #Windows requires protobuf in DLLs
    target_compile_definitions(${ENVIRONMENT_MANAGER} PRIVATE -DPROTOBUF_USE_DLLS)
    target_compile_definitions(${EXECUTION_TESTER} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
    target_link_libraries(${ENVIRONMENT_MANAGER} pthread)
    target_link_libraries(${ENVIRONMENT_MANAGER} dl)

    target_link_libraries(${EXECUTION_TESTER} pthread)
    target_link_libraries(${EXECUTION_TESTER} dl)
endif(MSVC)

#Link executable against all libraries required
target_link_libraries(${ENVIRONMENT_MANAGER} ${ZMQ_LIBRARIES})
target_link_libraries(${ENVIRONMENT_MANAGER} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${ENVIRONMENT_MANAGER} ${PROTOBUF_LIBRARIES})
target_link_libraries(${ENVIRONMENT_MANAGER} environment_proto_message)


target_link_libraries(reg_tester ${ZMQ_LIBRARIES})
target_link_libraries(reg_tester ${PROTOBUF_LIBRARIES})
target_link_libraries(reg_tester environment_proto_message)
