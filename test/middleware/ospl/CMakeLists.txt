#Set our project names
set(PROJ_NAME test_ospl)
project(${PROJ_NAME})

#Find protobuf
find_package(Protobuf REQUIRED)


# Find package OSPL
find_package(OSPL)

if(NOT OSPL_FOUND)
	message(STATUS "Cannot find OSPL cannot build project ${PROJ_NAME}")
	return()
endif()


# Copy this <idl> file into the binary directory so the compilation of generated files can succeed
configure_file(basic.idl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Generate the <ospl> files for Aggregate
OSPL_GENERATE_CPP(OSPL_SRCS OSPL_HDRS ${CMAKE_CURRENT_BINARY_DIR}/basic.idl)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ospltest.cpp
)
add_executable(${PROJ_NAME} ${SOURCE} ${RTI_SRCS} ${RTI_HDRS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${OSPL_LIBRARIES})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} ${OSPL_LIBRARIES})
target_link_libraries(${PROJ_NAME} ospl_helper)
