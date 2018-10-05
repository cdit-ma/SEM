cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJ_NAME zmq_protorequester)
project (${PROJ_NAME})

#find_package(Protobuf REQUIRED)
#find_package(ZMQ REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protorequester.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protorequester.hpp
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})


#Link against protobuf
#target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})
#target_link_libraries(${PROJ_NAME} PUBLIC ${ZMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoregister)
