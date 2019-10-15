cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(LOGAN_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(LOGAN_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LOGAN_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")

set(LOGAN_SOURCE_DIR "${LOGAN_SOURCE_DIR}" PARENT_SCOPE)
set(LOGAN_BINARY_DIR "${LOGAN_BINARY_DIR}" PARENT_SCOPE)

SET(LOGAN_VERSION "3.3.8")

#Add Options for building projects, default to build everything
option(BUILD_SERVER "Build logan_server" ON)
option(BUILD_CLIENT "Build logan_client" ON)
option(BUILD_AGGREGATIONSERVER "Build aggregation_server" ON)
option(DISABLE_MODEL_LOGGING "Disable model logging tables in logan_server")
option(DISABLE_HARDWARE_LOGGING "Disable hardware logging tables in logan_server")

#Build the source
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")
