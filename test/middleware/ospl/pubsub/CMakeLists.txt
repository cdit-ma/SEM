#Set our project names
set(PROJ_NAME test_ospl)
project(${PROJ_NAME})

if(NOT OSPL_DDS_FOUND)
	# Find package OSPL_DDS
	find_package(OSPL_DDS)
	if(NOT OSPL_DDS_FOUND)
		message(STATUS "Cannot find OSPL_DDS cannot build ${PROJ_NAME}")
		return()
	endif()
endif()

find_package(Boost 1.30.0 REQUIRED)


# Copy this <idl> file into the binary directory so the compilation of generated files can succeed
configure_file(../basic.idl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Generate the <ospl> files for Aggregate
OSPL_DDS_GENERATE_CPP(OSPL_SRCS OSPL_HDRS ${CMAKE_CURRENT_BINARY_DIR}/basic.idl)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/../translator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ospltest.cpp
)

add_executable(${PROJ_NAME} ${SOURCE} ${OSPL_SRCS} ${OSPL_HDRS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${OSPL_DDS_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

message(STATUS "OSPL_DDS_LIBRARIES: ${OSPL_DDS_LIBRARIES}")

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} ospl_dds_helper)
target_link_libraries(${PROJ_NAME} ${OSPL_DDS_LIBRARIES})
