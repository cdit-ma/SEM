set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Find ospl library
find_package(OSPL_DDS)

set(PROJ_NAME ospl_dds_helper)
project (${PROJ_NAME})

if(NOT OSPL_DDS_FOUND)
    message(STATUS "Not Found OSPL_DDS - Not building OSPL Middleware library")
    return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PRIVATE ${OSPL_DDS_INCLUDE_DIRS})

target_link_libraries(${PROJ_NAME} ${OSPL_DDS_LIBRARIES})
