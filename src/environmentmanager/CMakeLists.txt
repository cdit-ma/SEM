cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENVIRONMENT_MANAGER re_environment_manager)
set(ENVIRONMENT_MANAGER_LIB "${ENVIRONMENT_MANAGER}_lib")

set(PROJ_NAME ${ENVIRONMENT_MANAGER})
project(${PROJ_NAME})

find_package(ZMQ REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/experiment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/ddsport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/qpidport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/taoport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/zmqport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uniquequeue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.h
    ${CMAKE_CURRENT_SOURCE_DIR}/environment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/experiment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/ddsport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/qpidport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/taoport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/zmqport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.h
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
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${ENVIRONMENT_MANAGER_LIB} PRIVATE ${RE_COMMON_SOURCE_DIR})


target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${RE_SRC_PATH})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${ENVIRONMENT_MANAGER} PRIVATE ${RE_COMMON_SOURCE_DIR})



#Link executable against all libraries required
target_link_libraries(${ENVIRONMENT_MANAGER} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} ${ZMQ_LIBRARIES})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} ${PROTOBUF_LIBRARIES})
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} re_common_proto_control)
target_link_libraries(${ENVIRONMENT_MANAGER_LIB} zmq_protoreplier)
target_link_libraries(${ENVIRONMENT_MANAGER} ${ENVIRONMENT_MANAGER_LIB})

#target_link_libraries(rule_tester ${ENVIRONMENT_MANAGER_LIB})
#target_link_libraries(rule_tester re_execution_parser_lib)
