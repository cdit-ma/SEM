cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")

set(PROJ_NAME utility_worker)

project(${PROJ_NAME})


set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/utilityworker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utilityworker_impl.cpp
    )

#Add RE_PATH to include directories
include_directories("${RE_PATH}/src")

#Use this in production
add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)

target_link_libraries(${PROJ_NAME} re_core)
