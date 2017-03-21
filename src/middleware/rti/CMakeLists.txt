set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

#Find rti library
find_package(RTIDDS)

set(PROJ_NAME rti_helper)
project (${PROJ_NAME})

if(NOT RTIDDS_FOUND)
    message(" ------ RTI DDS NOT FOUND - NOT BUILDING RTI MIDDLEWARE LIBRARY ------ ")
    return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

include_directories("${RE_PATH}/includes")
include_directories(${RTIDDS_INCLUDE_DIRS})

# Need this to make sure headers are using the correct function calls.
target_compile_definitions(${PROJ_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)

target_link_libraries(${PROJ_NAME} ${RTIDDS_LIBRARIES})

