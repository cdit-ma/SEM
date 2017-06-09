# Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME "base_aggregate")
project(${PROJ_NAME})
# Find library re_core
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/aggregate.cpp
	)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/aggregate.h
	)

# Include the RE_PATH directory
include_directories("${RE_PATH}/src")

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
