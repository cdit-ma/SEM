#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")

set(PROJ_NAME components_hellosender)
project (${PROJ_NAME})

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/hellosenderimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hellosenderint.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/hellosenderimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hellosenderint.h
	)

#Add RE_PATH to include directories
include_directories("${RE_PATH}/includes")

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} datatypes_message)