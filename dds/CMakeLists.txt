cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
MESSAGE("${CMAKE_MODULE_PATH}")

set(PROJ_NAME dds_library)
project (${PROJ_NAME})

find_package(DDS REQUIRED)
include_directories(${PROJ_NAME} ${DDS_INCLUDE_DIRS})