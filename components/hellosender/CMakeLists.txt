#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")

#Set the projects RPATH to look in the executable directory
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "@executable_path;@loader_path;$ORIGIN;")

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

add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} datatypes_message)