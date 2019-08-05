
#Set our project names
set(PROJ_NAME re_common_proto_aggregation_message)
project(${PROJ_NAME})

find_package(Protobuf REQUIRED)

if(NOT PROTOBUF_FOUND)
	# Find package Protobuf
	find_package(Protobuf REQUIRED)
endif()

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/aggregationmessage.proto)

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS} ${PROTO_SRCS} ${PROTO_HDRS})

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_BINARY_DIR})


if (MSVC)
	#Windows requires protobuf in DLLs
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

# Link Libraries 
target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})