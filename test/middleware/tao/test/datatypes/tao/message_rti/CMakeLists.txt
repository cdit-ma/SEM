set(PROJ_NAME "rti_message")
project(${PROJ_NAME})

# Find package RTIDDS
find_package(RTIDDS)

if(NOT RTIDDS_FOUND)
	message(STATUS "Cannot find RTIDDS cannot build ${PROJ_NAME}")
	return()
endif()

# Find library re_core
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

# Find library rti_helper
find_library(RTI_HELPER_LIBRARIES rti_helper "${RE_PATH}/lib")

set(SHARED_LIBRARY_NAME "rti_message_lib")
# Copy the <idl> file into the binary directory so it can be used by the middleware compilers
configure_file(message.idl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Run the rti Compiler over the <idl> files
RTI_GENERATE_CPP(RTI_SOURCES RTI_HEADER ${CMAKE_CURRENT_BINARY_DIR}/message.idl)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/convert.h
)

add_library(${SHARED_LIBRARY_NAME} SHARED ${SOURCE} ${HEADERS} ${RTI_SOURCES} ${RTI_HEADER})
add_library(${PROJ_NAME} MODULE ${CMAKE_CURRENT_SOURCE_DIR}/libportexport.cpp)
# Include the runtime environment directory
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${RE_PATH}/src")
# Include the middleware include directory
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${RTIDDS_INCLUDE_DIRS}")
# Include the current binary directory to allow inclusion of generated files
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# Include required aggregates source dirs
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
# Set RTI Definitions
target_compile_definitions(${PROJ_NAME} PRIVATE "-DRTI_UNIX -DRTI_64BIT")
target_compile_definitions(${SHARED_LIBRARY_NAME} PRIVATE "-DRTI_UNIX -DRTI_64BIT")
# Link against runtime environment
target_link_libraries(${SHARED_LIBRARY_NAME} "${RE_CORE_LIBRARIES}")
# Link against the middleware libraries
target_link_libraries(${SHARED_LIBRARY_NAME} "${RTIDDS_LIBRARIES}")
# Link against the base aggregate
target_link_libraries(${SHARED_LIBRARY_NAME} "base_message_lib")
target_include_directories(${PROJ_NAME} PRIVATE "${RE_PATH}/src")
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_include_directories(${PROJ_NAME} PRIVATE "${RTIDDS_INCLUDE_DIRS}")
# Include the current binary directory to allow inclusion of generated files
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${PROJ_NAME} "${RTIDDS_LIBRARIES}")
target_link_libraries(${PROJ_NAME} "${RE_CORE_LIBRARIES}")
target_link_libraries(${PROJ_NAME} "${RTI_HELPER_LIBRARIES}")
target_link_libraries(${PROJ_NAME} "rti_message_lib")

