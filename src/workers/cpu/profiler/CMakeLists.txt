cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJ_NAME cpu_profiler)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_profiler.cpp
    )

    
add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} cpu_worker)