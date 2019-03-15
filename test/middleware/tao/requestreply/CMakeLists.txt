#Set our project names
set(PROJ_NAME test_tao_requestreply)
project(${PROJ_NAME})

# Find package ZMQ
find_package(TAO)

if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()


find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
)

TAO_GENERATE_CPP(TAO_SRCS TAO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/../message.idl)

add_executable(${PROJ_NAME} "")

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
    ${TAO_SRCS}

    # Headers
    ${TAO_HDRS}
)
# TODO

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SOURCE_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif()

target_link_libraries(${PROJ_NAME} PRIVATE ${TAO_LIBRARIES})

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJ_NAME} PRIVATE tao_helper)
target_link_libraries(${PROJ_NAME} PRIVATE re_environment_controller_lib)
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARY})