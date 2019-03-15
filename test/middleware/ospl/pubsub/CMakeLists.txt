set(PROJ_NAME test_ospl_pubsub)
project(${PROJ_NAME})

if(NOT OSPL_DDS_FOUND)
	# Find package OSPL_DDS
	find_package(OSPL_DDS)
	if(NOT OSPL_DDS_FOUND)
		message(STATUS "Cannot find OSPL_DDS cannot build ${PROJ_NAME}")
		return()
	endif()
endif()

OSPL_DDS_GENERATE_CPP(OSPL_SRCS OSPL_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/../basic.idl)

add_executable(${PROJ_NAME})

target_sources(${PROJ_NAME} PRIVATE
	# Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
	${OSPL_SRCS}
	
	# Headers
	${OSPL_HDRS}
)

#target_include_directories(${PROJ_NAME} PRIVATE ${RE_SOURCE_PATH})
#target_include_directories(${PROJ_NAME} PRIVATE ${OSPL_DDS_INCLUDE_DIRS})
#target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJ_NAME} PRIVATE ospl_dds_helper)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)
