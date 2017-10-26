cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJ_NAME idl2graphml)
set(LIB_NAME medea_idl2graphml_lib)

project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlkinds.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlexporter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/idlparser.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlkinds.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlexporter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/idlparser.h
)

include_directories(${Boost_INCLUDE_DIRS})
add_library(${LIB_NAME} STATIC ${SOURCE} ${HEADERS})
add_executable(${PROJ_NAME} main.cpp)
target_link_libraries(${PROJ_NAME} ${LIB_NAME})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})

INSTALL(TARGETS ${PROJ_NAME} RUNTIME DESTINATION . COMPONENT ${PROJ_NAME})