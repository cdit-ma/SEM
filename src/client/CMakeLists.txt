set(LOGAN_CLIENT "logan_client")
set(LOGAN_CLIENT_LIB ${LOGAN_CLIENT}_lib)
set(PROJ_NAME ${LOGAN_CLIENT})

project(${PROJ_NAME})

#Find packages
find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)
find_package(SIGAR REQUIRED)

#Need boost > 1.30.0 system and filesystem for crossplatform temp file creation.
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

#Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/sigarsysteminfo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/logcontroller.cpp
)

set(EXEC_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/sigarsysteminfo.h
	${CMAKE_CURRENT_SOURCE_DIR}/logcontroller.h
)

# Construct a server binary
add_executable(${LOGAN_CLIENT} ${EXEC_SOURCE})

# Construct a shared library
add_library(${LOGAN_CLIENT_LIB} SHARED ${SOURCE} ${HEADERS})

# Link against boost dynamically
target_compile_definitions(${LOGAN_CLIENT} PRIVATE -DBOOST_ALL_DYN_LINK)

if (MSVC)
    # Windows requires protobuf in DLLs
	add_definitions(-DPROTOBUF_USE_DLLS)
	# Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
else()
	# Unix needs to include pthread
	target_link_libraries(${LOGAN_CLIENT_LIB} pthread)
    target_link_libraries(${LOGAN_CLIENT_LIB} dl)
endif()


#include the current binary directory to re_node_manager as it requires the .h file from the pb generated files
target_include_directories(${LOGAN_CLIENT} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${LOGAN_CLIENT} PRIVATE ${RE_COMMON_SOURCE_DIR})

target_include_directories(${LOGAN_CLIENT_LIB} PRIVATE ${SIGAR_INCLUDE_DIRS})
target_include_directories(${LOGAN_CLIENT_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${LOGAN_CLIENT_LIB} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${LOGAN_CLIENT_LIB} PRIVATE ${LOGAN_SOURCE_DIR})

target_include_directories(${LOGAN_CLIENT} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${LOGAN_CLIENT_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${LOGAN_CLIENT_LIB} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${LOGAN_CLIENT_LIB} PRIVATE ${RE_COMMON_SOURCE_DIR})


target_link_libraries(${LOGAN_CLIENT_LIB} ${PROTOBUF_LIBRARIES})
target_link_libraries(${LOGAN_CLIENT_LIB} re_common_proto_systemstatus)
target_link_libraries(${LOGAN_CLIENT_LIB} re_common_proto_control)
target_link_libraries(${LOGAN_CLIENT_LIB} zmq_protowriter)
target_link_libraries(${LOGAN_CLIENT_LIB} ${SIGAR_LIBRARIES})


target_link_libraries(${LOGAN_CLIENT} ${LOGAN_CLIENT_LIB})
target_link_libraries(${LOGAN_CLIENT} ${Boost_PROGRAM_OPTIONS_LIBRARY})