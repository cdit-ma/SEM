#Set our project names
set(PROJ_NAME test_rti)
project(${PROJ_NAME})

#Find protobuf
find_package(Protobuf REQUIRED)

# Find package RTIDDS
find_package(RTI_DDS)

if(NOT RTI_DDS_FOUND)
	message(STATUS "Cannot find RTI_DDS cannot build project ${PROJ_NAME}")
	return()
endif()

# Generate the <rti> files for Aggregate
RTI_DDS_GENERATE_CPP(RTI_SRCS RTI_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/../basic.idl)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/../translator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${GOOGLE_TEST_DIR}/src/gtest_main.cc
)
add_executable(${PROJ_NAME} ${SOURCE} ${RTI_SRCS} ${RTI_HDRS})

# Need this to make sure headers are using the correct function calls.
target_compile_definitions(${PROJ_NAME} PRIVATE ${RTI_DDS_DEFINITIONS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${RTI_DDS_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} ${RTI_DDS_LIBRARIES})
target_link_libraries(${PROJ_NAME} rti_dds_helper)