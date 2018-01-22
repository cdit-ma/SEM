cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


if (UNIX)
    SET(NO_OPTIMIZE_FLAG "-O0")
elseif(MSVC)
    SET(NO_OPTIMIZE_FLAG "-Od /wd4996 /wd4244")
endif(UNIX)

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${NO_OPTIMIZE_FLAG}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${NO_OPTIMIZE_FLAG}")

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${NO_OPTIMIZE_FLAG}")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${NO_OPTIMIZE_FLAG}")


set(PROJ_NAME cpu_worker)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/floatop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry_1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/whets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry.h
    )


add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)

target_link_libraries(${PROJ_NAME} re_core)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/profiler")