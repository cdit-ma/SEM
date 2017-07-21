cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{CMAKE_MODULE_PATH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(PROJ_NAME "re_execution_parser")
set(LIB_NAME "re_execution_parser_lib")
project(${PROJ_NAME})

set(PROJ_NAME re_execution_parser)

project(${PROJ_NAME})

#Find PUGIXML Library
set(PUGIXML_ROOT $ENV{PUGIXML_ROOT})
find_library(PUGIXML_LIBRARIES pugixml ${PUGIXML_ROOT}/lib)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/modelparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/modelparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes.h
    )

include_directories(${PUGIXML_ROOT}/include)

add_library(${LIB_NAME} ${SOURCE} ${HEADERS})
add_executable(${PROJ_NAME} "main.cpp")

target_link_libraries(${LIB_NAME} ${PUGIXML_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${LIB_NAME})