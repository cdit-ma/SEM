set(PROJ_NAME "tao_base_message_lib")
project(${PROJ_NAME})

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/message.cpp
)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/message.h
)

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

# Include the runtime environment directory
target_include_directories(${PROJ_NAME} PRIVATE "${RE_PATH}/src")
# Link against runtime environment
target_link_libraries(${PROJ_NAME} re_core)

