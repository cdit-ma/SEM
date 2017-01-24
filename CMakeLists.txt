cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set the projects RPATH to look in the executable directory
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "@executable_path;@loader_path;$ORIGIN;")

set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

#Add all bits for Deployment
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message/zmq")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message/proto")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/components/hellosender")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/components/helloreceiver")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deployment_1")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deployment_2")