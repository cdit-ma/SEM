set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

#Find rti library
find_package(QPID)

set(PROJ_NAME qpid_helper)
project (${PROJ_NAME})

if(NOT QPID_FOUND)
    message(STATUS "Not Found QPID - Not building QPID Middleware library")
    return()
endif()

set(SOURCE
    qpidhelper.cpp
	)

set(HEADERS
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

include_directories("${RE_PATH}/includes")
include_directories(${QPID_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} ${QPID_LIBRARIES})

