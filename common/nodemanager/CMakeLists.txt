cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJ_NAME re_node_manager)
project (${PROJ_NAME})

set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/common")
include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/nodelauncher.cpp
	)

add_executable(${PROJ_NAME} ${SOURCE})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()