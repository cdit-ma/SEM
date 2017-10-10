cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


#Requires Qt5_DIR and QTIFWDIR to be set
set(PROJ_NAME idl2graphml)

project(${PROJ_NAME})


set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlexporter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/idlparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${PROJ_NAME} ${SOURCE})