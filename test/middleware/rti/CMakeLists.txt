#Set our project names
set(PROJ_NAME test_rti)
project(${PROJ_NAME})

#Find protobuf
find_package(Protobuf REQUIRED)

# Find package RTIDDS
find_package(RTIDDS)

if(NOT RTIDDS_FOUND)
	message(STATUS "Cannot find RTIDDS cannot build project ${PROJ_NAME}")
	return()
endif()

# Generate the <rti> files for Aggregate
RTI_GENERATE_CPP(RTI_SRCS RTI_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/basic.idl)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/rtitest.cpp
)
add_executable(${PROJ_NAME} ${SOURCE} ${RTI_SRCS} ${RTI_HDRS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${RTIDDS_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

# Need this to make sure headers are using the correct function calls.
target_compile_definitions(${PROJ_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)




target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} ${RTIDDS_LIBRARIES})
target_link_libraries(${PROJ_NAME} rti_helper)
