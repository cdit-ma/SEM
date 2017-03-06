cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")

SET(CMAKE_CXX_FLAGS "-O0")
SET(CMAKE_C_FLAGS "-O0")

set(PROJ_NAME cpu_worker)

project(${PROJ_NAME})


set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/WE_CPU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WE_CPU_Impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/floatop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry_1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/whets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry.h
    )

#Add RE_PATH to include directories
include_directories("${RE_PATH}/src")

#Use this in production
#add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

#Use this for enable_testing
add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
add_executable(test main.cpp)

if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)

target_link_libraries(${PROJ_NAME} re_core)

target_link_libraries(test ${PROJ_NAME})
