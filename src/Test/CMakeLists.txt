cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_MACOSX_RPATH 1)

set(PROJ_NAME MEDEA_test)
find_package(Qt5 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modelcontrollertests.cpp
)

SET(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/modelcontrollertests.h
)

set(RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/models.qrc
    )

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS} ${RESOURCES} ${MAC_ICON})

add_executable(medea_cmd cmd.cpp utils.cpp)

target_link_libraries(${PROJ_NAME} Qt5::Test)
target_link_libraries(${PROJ_NAME} medea_modelcontroller)
target_link_libraries(medea_cmd medea_modelcontroller)
