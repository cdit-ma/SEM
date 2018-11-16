set(PROJ_NAME tao_helper)
project (${PROJ_NAME})

find_package(TAO)

if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/taohelper.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/taohelper.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PUBLIC ${TAO_INCLUDE_DIRS})

target_link_libraries(${PROJ_NAME} PUBLIC ${TAO_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC re_core)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utils")