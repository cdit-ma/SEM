set(LOGAN_SERVER_LIB logan_server_lib)

project(${LOGAN_SERVER_LIB})

#Find packages
find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

#Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protohandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/table.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h
)

# Construct a shared library
add_library(${LOGAN_SERVER_LIB} SHARED ${SOURCE} ${HEADERS})

if (MSVC)
    # Windows requires protobuf in DLLs
	add_definitions(-DPROTOBUF_USE_DLLS)
	# Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
else()
	# Unix needs to include pthread
	target_link_libraries(${LOGAN_SERVER_LIB} pthread)
    target_link_libraries(${LOGAN_SERVER_LIB} dl)
endif()


target_include_directories(${LOGAN_SERVER_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${LOGAN_SERVER_LIB} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${LOGAN_SERVER_LIB} PRIVATE ${LOGAN_SRC_PATH})

target_include_directories(${LOGAN_SERVER_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${LOGAN_SERVER_LIB} PRIVATE ${RE_COMMON_BINARY_DIR})

target_link_libraries(${LOGAN_SERVER_LIB} zmq_protoreceiver)
target_link_libraries(${LOGAN_SERVER_LIB} re_common_proto_systemstatus)
target_link_libraries(${LOGAN_SERVER_LIB} re_common_proto_control)

if(NOT DISABLE_MODEL_LOGGING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/modelprotohandler")
    target_link_libraries(${LOGAN_SERVER_LIB} modelprotohandler)
else()
    target_compile_definitions(${LOGAN_SERVER_LIB} PRIVATE DISABLE_MODEL_HANDLER)
endif()

if(NOT DISABLE_HARDWARE_LOGGING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/hardwareprotohandler")
    target_link_libraries(${LOGAN_SERVER_LIB} hardwareprotohandler)
else()
    target_compile_definitions(${LOGAN_SERVER_LIB} PRIVATE DISABLE_HARDWARE_HANDLER)
endif()


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/managedserver")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/standaloneserver")
