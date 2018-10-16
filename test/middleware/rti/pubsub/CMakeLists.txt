#Set our project names
set(PROJ_NAME test_rti_pubsub)
project(${PROJ_NAME})

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
    ${RE_TEST_SOURCE_DIR}/core/test_main.cpp
)
add_executable(${PROJ_NAME} ${SOURCE} ${RTI_SRCS} ${RTI_HDRS})

# Need this to make sure headers are using the correct function calls.
target_compile_definitions(${PROJ_NAME} PRIVATE ${RTI_DDS_DEFINITIONS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SOURCE_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${RTI_DDS_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif()

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE ${RTI_DDS_LIBRARIES})
target_link_libraries(${PROJ_NAME} PRIVATE rti_dds_helper)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)