set(PROJ_NAME tao_helper)
project (${PROJ_NAME})
set(TAO_HELPER_LIBRARIES ${PROJ_NAME})

# Find package TAO
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

if (MSVC)
    #Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

target_link_libraries(${PROJ_NAME} ${TAO_LIBRARIES})
target_link_libraries(${PROJ_NAME} re_core)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utils")