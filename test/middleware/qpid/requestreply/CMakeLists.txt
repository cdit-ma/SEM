#Set our project names
set(PROJ_NAME test_qpid_requestreply)
project(${PROJ_NAME})

#Find protobuf
find_package(Protobuf REQUIRED)

# Find package QPID
find_package(QPID)

if(NOT QPID_FOUND)
	message(STATUS "Cannot find QPID cannot build project ${PROJ_NAME}")
	return()
endif()

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(${PROJ_NAME} "")

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
)

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE qpid_helper)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)
target_link_libraries(${PROJ_NAME} PRIVATE re_environment_controller_lib)
target_link_libraries(${PROJ_NAME} PRIVATE lib_test_basic_proto)
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARY})