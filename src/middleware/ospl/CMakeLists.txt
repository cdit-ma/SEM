set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

#Find ospl library
find_package(OpenSplice REQUIRED)

set(PROJ_NAME ospl_helper)
project (${PROJ_NAME})

if(NOT OpenSplice)
    message(" ------ OpenSplice NOT FOUND - NOT BUILDING OpenSplice MIDDLEWARE LIBRARY ------ ")
    return()
endif()

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

include_directories("${RE_PATH}/includes")
include_directories(${OpenSplice_INCLUDE_DIRS})

target_link_libraries(${PROJ_NAME} ${OpenSplice_LIBRARIES})

