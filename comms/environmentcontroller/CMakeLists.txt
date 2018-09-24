cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJ_NAME re_environment_controller)
set(LIB_NAME ${PROJ_NAME}_lib)
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

set(LIB_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentcontroller.cpp
    )

set(LIB_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentcontroller.h
    )

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )

add_executable(${PROJ_NAME} ${SOURCE})
add_library(${LIB_NAME} SHARED ${LIB_SOURCE} ${LIB_HEADERS})

target_compile_definitions(${PROJ_NAME} PRIVATE -DBOOST_ALL_DYN_LINK)

target_include_directories(${LIB_NAME} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_BINARY_DIR})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${LIB_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(${LIB_NAME} re_common_proto_environmentcontrol)
target_link_libraries(${LIB_NAME} re_common_proto_control)
target_link_libraries(${LIB_NAME} zmq_protorequester)
target_link_libraries(${LIB_NAME} re_graphml_parser_lib)

target_link_libraries(${PROJ_NAME} ${LIB_NAME})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})


