
set(PROJ_NAME rti_dds_helper)
project (${PROJ_NAME})

# Find RTI
find_package(RTI_DDS)

if(NOT RTI_DDS_FOUND)
    message(STATUS "Cannot find RTI_DDS cannot build project ${PROJ_NAME}")
    return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

# Compile Definitions
target_compile_definitions(${PROJ_NAME} PUBLIC ${RTI_DDS_DEFINITIONS})

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${RTI_DDS_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC ${RTI_DDS_LIBRARIES})