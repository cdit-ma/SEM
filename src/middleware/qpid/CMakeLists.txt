
set(PROJ_NAME qpid_helper)
project (${PROJ_NAME})

find_package(QPID)

if(NOT QPID_FOUND)
    message(STATUS "Cannot find QPID cannot build project ${PROJ_NAME}")
    return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/qpidhelper.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/qpidhelper.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${QPID_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC ${QPID_LIBRARIES})
