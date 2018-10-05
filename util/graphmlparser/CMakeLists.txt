cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)

set(EXECUTION_PARSER "re_graphml_parser")
set(EXECUTION_PARSER_LIB ${EXECUTION_PARSER}_lib)

set(PROJ_NAME ${EXECUTION_PARSER})
project(${PROJ_NAME})

#Find PUGIXML Library
set(PUGIXML_ROOT $ENV{PUGIXML_ROOT})

if(NOT PUGIXML_LIBRARIES)
    # Find library PUGIXML_LIBRARIES
    find_library(PUGIXML_LIBRARIES pugixml REQUIRED "${PUGIXML_ROOT}/lib")
    
	if(NOT PUGIXML_LIBRARIES)
		message(STATUS "Cannot find PUGIXML cannot build ${PROJ_NAME}")
		return()
    endif()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/modelparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/protobufmodelparser.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/modelparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/protobufmodelparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes.h
)

add_library(${EXECUTION_PARSER_LIB} STATIC ${SOURCE} ${HEADERS})
add_executable(${EXECUTION_PARSER} "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${PUGIXML_ROOT}/include)

# Remove the need for _lib extensions
target_link_libraries(${EXECUTION_PARSER_LIB} PRIVATE ${PUGIXML_LIBRARIES})
target_link_libraries(${EXECUTION_PARSER_LIB} PUBLIC re_common_proto_control)


target_link_libraries(${EXECUTION_PARSER} ${EXECUTION_PARSER_LIB})

