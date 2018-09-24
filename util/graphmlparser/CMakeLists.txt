cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

find_package(Protobuf REQUIRED)

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

add_library(${EXECUTION_PARSER_LIB} SHARED ${SOURCE} ${HEADERS})
add_executable(${EXECUTION_PARSER} "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    #Windows requires protobuf in DLLs
    target_compile_definitions(${EXECUTION_PARSER_LIB} PRIVATE -DPROTOBUF_USE_DLLS)
endif(MSVC)


target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${PUGIXML_ROOT}/include)
target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${RE_SRC_PATH})


target_include_directories(${EXECUTION_PARSER} PRIVATE ${PUGIXML_ROOT}/include)
target_include_directories(${EXECUTION_PARSER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${EXECUTION_PARSER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${EXECUTION_PARSER} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${EXECUTION_PARSER} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${EXECUTION_PARSER} PRIVATE ${RE_SRC_PATH})


# Remove the need for _lib extensions
target_link_libraries(${EXECUTION_PARSER_LIB} re_common_proto_control)
target_link_libraries(${EXECUTION_PARSER_LIB} ${PUGIXML_LIBRARIES})
target_link_libraries(${EXECUTION_PARSER_LIB} ${PROTOBUF_LIBRARIES})

target_link_libraries(${EXECUTION_PARSER} ${EXECUTION_PARSER_LIB})
