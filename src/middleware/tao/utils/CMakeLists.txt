set(PROJ_NAME tao_nameserver_list)
project (${PROJ_NAME})
set(TAO_HELPER_LIBRARIES ${PROJ_NAME})

# Find package TAO
find_package(TAO)


if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/nameserverrequester.cpp
)

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} tao_helper)
