cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")


set(PROJ_NAME execution_parser)
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

add_library(${PROJ_NAME} ${SOURCE} ${HEADERS})
add_executable("test" "main.cpp")

if (MSVC)
    #Do Nothing

elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif(MSVC)

target_link_libraries(${PROJ_NAME} ${PUGIXML_LIBRARIES})
target_link_libraries("test" ${PROJ_NAME})