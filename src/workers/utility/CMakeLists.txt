cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJ_NAME utility_worker)
project(${PROJ_NAME})
find_package(Boost 1.30.0 REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/exprtk.hpp
    )

    
add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /bigobj")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)

target_link_libraries(${PROJ_NAME} re_core)
