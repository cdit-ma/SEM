#Set our project names
set(PROJ_NAME re_proto_control)

if(NOT PROTOBUF_FOUND)
	# Find package Protobuf
	find_package(Protobuf REQUIRED)
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.h
	)

#Generate out protobuf for control
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/controlmessage.proto)


add_library(${PROJ_NAME} STATIC ${PROTO_SRCS} ${SOURCE} ${PROTO_HDRS} ${HEADERS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (MSVC)
	#Windows requires protobuf in DLLs
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

#Link against protobuf
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})

