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

add_executable(${PROJ_NAME} "")

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${RTI_SRCS}
    # Headers
    ${RTI_HDRS}
)

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJ_NAME} PRIVATE ${RTI_DDS_LIBRARIES})
target_link_libraries(${PROJ_NAME} PRIVATE rti_dds_helper)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)