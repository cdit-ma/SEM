set(PROJ_NAME ospl_dds_helper)
project (${PROJ_NAME})

find_package(OSPL_DDS)

if(NOT OSPL_DDS_FOUND)
    message(STATUS "Cannot find OSPL_DDS cannot build project ${PROJ_NAME}")
    return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${OSPL_DDS_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC ${OSPL_DDS_LIBRARIES})