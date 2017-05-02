cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#find_package(Qt5Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

SET(PROJ_NAME "Controller_Test")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/controllertest.cpp
    

)

set(HEADERS
)

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})
target_link_libraries(${PROJ_NAME} MEDEA_MODELCONTROLLER)
#target_link_libraries(${PROJ_NAME} Qt5::Test)