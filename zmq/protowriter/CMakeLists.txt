cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJ_NAME zmq_protowriter)

project (${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS filesystem system REQUIRED)
find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protowriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cachedprotowriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/monitor.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protowriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cachedprotowriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/monitor.h
)

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})


target_compile_definitions(${PROJ_NAME} PUBLIC BOOST_ALL_DYN_LINK)
target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)

target_include_directories(${PROJ_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${ZMQ_INCLUDE_DIRS})



target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC ${ZMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC ${Boost_SYSTEM_LIBRARY})
target_link_libraries(${PROJ_NAME} PUBLIC ${Boost_FILESYSTEM_LIBRARY})