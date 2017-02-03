cmake_minimum_required (VERSION 2.6)

#Set the binary and library locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/includes/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/includes/opensplice")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/includes/rti")
