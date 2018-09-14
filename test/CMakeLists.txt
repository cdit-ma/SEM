cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)

#Set our project names
set(PROJ_NAME tester)
set(TX_NAME test_tx)
set(RX_NAME test_rx)

if(NOT PROTOBUF_FOUND)
	# Find package Protobuf
	find_package(Protobuf REQUIRED)
endif()

set(TX_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/tx.cpp
)

set(RX_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/rx.cpp
)


add_executable(${TX_NAME} ${TX_SOURCE})
add_executable(${RX_NAME} ${RX_SOURCE})


target_include_directories(${TX_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${TX_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../)
target_include_directories(${TX_NAME} PRIVATE ${RE_COMMON_PATH})

target_include_directories(${RX_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${RX_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../)
target_include_directories(${RX_NAME} PRIVATE ${RE_COMMON_PATH})

if (MSVC)
	#Windows requires protobuf in DLLs
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

#Link against protobuf
target_link_libraries(${TX_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${TX_NAME} zmq_protorequester)
target_link_libraries(${TX_NAME} re_common_proto_systemstatus)
target_link_libraries(${TX_NAME} re_common_proto_modelevent)
target_link_libraries(${TX_NAME} re_common_proto_control)

#Link against protobuf
target_link_libraries(${RX_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${RX_NAME} zmq_protoreplier)
target_link_libraries(${RX_NAME} re_common_proto_systemstatus)
target_link_libraries(${RX_NAME} re_common_proto_modelevent)
target_link_libraries(${RX_NAME} re_common_proto_control)