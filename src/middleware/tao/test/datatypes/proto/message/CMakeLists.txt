set(PROJ_NAME "proto_message")
project(${PROJ_NAME})

# Find package Protobuf
find_package(Protobuf)

if(NOT PROTOBUF_FOUND)
	message(STATUS "Cannot find Protobuf cannot build ${PROJ_NAME}")
	return()
endif()

# Find library re_core
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

set(SHARED_LIBRARY_NAME "proto_message_lib")
# Copy the <proto> file into the binary directory so it can be used by the middleware compilers
configure_file(message.proto ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Run the proto Compiler over the <proto> files
protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADER ${CMAKE_CURRENT_BINARY_DIR}/message.proto)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/convert.h
)

add_library(${SHARED_LIBRARY_NAME} SHARED ${SOURCE} ${HEADERS} ${PROTO_SOURCES} ${PROTO_HEADER})
# Include the runtime environment directory
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${RE_PATH}/src")
# Include the middleware include directory
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${PROTOBUF_INCLUDE_DIRS}")
# Include the current binary directory to allow inclusion of generated files
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# Include required aggregates source dirs
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
# Windows specific protobuf settings
if(MSVC)
	target_compile_definitions(${SHARED_LIBRARY_NAME} PRIVATE "-DPROTOBUF_USE_DLLS")
endif(MSVC)

message(${RE_CORE_LIBRARIES})
message(${PROTOBUF_LIBRARIES})
# Link against runtime environment
target_link_libraries(${SHARED_LIBRARY_NAME} "${RE_CORE_LIBRARIES}")
# Link against the middleware libraries
target_link_libraries(${SHARED_LIBRARY_NAME} "/usr/local/lib/libprotobuf.so")
# Link against the base aggregate
target_link_libraries(${SHARED_LIBRARY_NAME} "base_message_lib")
