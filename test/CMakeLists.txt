cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(Qt5Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

SET(PROJ_NAME "MEDEA_Test")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modelcontrollertests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/viewcontrollertests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modelelementtests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AdoptionTests/modeladoptiontest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AdoptionTests/interfacedefinitionsadoptiontest.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/modelcontrollertests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/viewcontrollertests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/modelelementtests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AdoptionTests/modeladoptiontest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AdoptionTests/interfacedefinitionsadoptiontest.h
)

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})
target_link_libraries(${PROJ_NAME} MEDEA_LIB)

target_link_libraries(${PROJ_NAME} Qt5::Test)