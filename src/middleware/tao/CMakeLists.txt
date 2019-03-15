set(PROJ_NAME tao_helper)
project (${PROJ_NAME})

find_package(TAO)

if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

add_library(${PROJ_NAME} SHARED)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/taohelper.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/taohelper.h
)

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${TAO_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC ${TAO_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC re_core)