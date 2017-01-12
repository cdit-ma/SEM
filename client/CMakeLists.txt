cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(PROJ_NAME logan_client)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(ZeroMQ REQUIRED)
include_directories(${PROJ_NAME} ${ZeroMQ_INCLUDE_DIRS})

find_package(SIGAR REQUIRED)
include_directories(${PROJ_NAME} ${SIGAR_INCLUDE_DIRS})

find_package(Boost 1.36.0 COMPONENTS program_options filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#Generate our Protobuf source/headers
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../common/systemstatus.proto)

#Link our Logger against all the Cpps
add_executable(${PROJ_NAME} main.cpp cachedzmqmessagewriter.cpp systemstatus.pb.cc sigarsysteminfo.cpp logcontroller.cpp zmqmessagewriter.cpp ${PROTO_SRCS})

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
target_link_libraries(${PROJ_NAME} ${SIGAR_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${PROJ_NAME} ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(${PROJ_NAME} ${Boost_SYSTEM_LIBRARY})
