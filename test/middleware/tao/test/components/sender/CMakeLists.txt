set(PROJ_NAME "component_sender")
project(${PROJ_NAME})
# Find library re_core
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/senderint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/libcomponentexport.cpp
)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/senderint.h
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.h
)

add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

# Include the runtime environment directory
target_include_directories(${PROJ_NAME} PRIVATE "${RE_PATH}/src")
# Link against runtime environment
target_link_libraries(${PROJ_NAME} re_core)

# Include required aggregates source dirs
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../datatypes/base")

# Link against required aggregates libraries
target_link_libraries(${PROJ_NAME} "base_message_lib")

