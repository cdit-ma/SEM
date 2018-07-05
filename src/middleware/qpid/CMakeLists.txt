set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Find rti library
find_package(QPID)

set(PROJ_NAME qpid_helper)
project (${PROJ_NAME})
set(QPID_HELPER_LIBRARIES ${PROJ_NAME})

if(NOT QPID_FOUND)
    message(STATUS "Not Found QPID - Not building QPID Helper library")
    return()
endif()



set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/qpidhelper.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/qpidhelper.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${QPID_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} ${QPID_LIBRARIES})
