set(PROJ_NAME tao_nameserver_list)
project (${PROJ_NAME})

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/nameserverrequester.cpp
)

add_executable(${PROJ_NAME} ${SOURCE})

target_include_directories(${PROJ_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_link_libraries(${PROJ_NAME} PRIVATE tao_helper)