#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")

#Set the projects RPATH to look in the executable directory
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "@executable_path;@loader_path;$ORIGIN;")

set(PROJ_NAME components_helloreceiver)
project (${PROJ_NAME})

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/helloreceiverimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/helloreceiverint.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/helloreceiverimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/helloreceiverint.h
	)

#Add RE_PATH to include directories
include_directories("${RE_PATH}/includes")

add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} datatypes_message)