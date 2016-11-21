cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(PROJ_NAME logan_server)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(ZeroMQ REQUIRED)
include_directories(${PROJ_NAME} ${ZeroMQ_INCLUDE_DIRS})

#Generate our Protobuf source/headers
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../common/systemstatus.proto)

#Link our Server against all the Cpps
add_executable(${PROJ_NAME} main.cpp systemstatus.pb.cc logdatabase.cpp sqlitedatabase.cpp sqlite3.c)


if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	#Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread dl)
endif()

#Link against our required libraries
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})


