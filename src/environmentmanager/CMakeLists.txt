cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENVIRONMENT_MANAGER environment_manager)
set(ENVIRONMENT_MANAGER_LIB "${ENVIRONMENT_MANAGER}_lib")

set(PROJ_NAME ${ENVIRONMENT_MANAGER})
project(${PROJ_NAME})

find_package(ZMQ REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/broadcaster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentgenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentrules/zmq/zmqrule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentrules/dds/ddsrule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentrules/amqp/amqprule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentrules/tao/taorule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/experiment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uniquequeue.hpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/broadcaster.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentgenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentrules/zmq/zmqrule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentrules/dds/ddsrule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentrules/amqp/amqprule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentrules/tao/taorule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.h
    ${CMAKE_CURRENT_SOURCE_DIR}/environment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/experiment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../re_common/util/execution.hpp
    )

#Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

add_library(${ENVIRONMENT_MANAGER_LIB} SHARED ${SOURCE} ${HEADERS})

add_executable(${ENVIRONMENT_MANAGER} ${CMAKE_CURRENT_SOURCE_DIR}/environmentmanager.cpp)
#add_executable(rule_tester ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)


if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    #Windows requires protobuf in DLLs
    target_compile_definitions(${ENVIRONMENT_MANAGER} PRIVATE -DBOOST_ALL_DYN_LINK)
    target_compile_definitions(${ENVIRONMENT_MANAGER} PRIVATE -DPROTOBUF_USE_DLLS)
    target_compile_definitions(${ENVIRONMENT_MANAGER_LIB} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
    target_link_libraries(${ENVIRONMENT_MANAGER} pthread)
    target_link_libraries(${ENVIRONMENT_MANAGER} dl)
endif(MSVC)

target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${RE_SRC_PATH})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../re_common/)

target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${RE_SRC_PATH})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../re_common/)



#Link executable against all libraries required
target_link_libraries(${ENVIRONMENT_MANAGER} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} ${ZMQ_LIBRARIES})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} ${PROTOBUF_LIBRARIES})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} re_common_proto_control)
target_link_libraries(${ENVIRONMENT_MANAGER} ${ENVIRONMENT_MANAGER_LIB})

#target_link_libraries(rule_tester ${ENVIRONMENT_MANAGER_LIB})
#target_link_libraries(rule_tester re_execution_parser_lib)
