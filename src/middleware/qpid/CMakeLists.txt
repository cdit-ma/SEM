set(PROJ_NAME qpid_helper)
project (${PROJ_NAME})

find_package(QPID)

if(NOT QPID_FOUND)
    message(STATUS "Cannot find QPID cannot build project ${PROJ_NAME}")
    return()
endif()

add_library(${PROJ_NAME} SHARED "")

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/qpidhelper.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/qpidhelper.h
)

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${QPID_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC ${QPID_LIBRARIES})