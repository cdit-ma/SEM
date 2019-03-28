set(EXECUTION_PARSER "re_graphml_parser")
set(EXECUTION_PARSER_LIB ${EXECUTION_PARSER}_lib)

set(PROJ_NAME ${EXECUTION_PARSER})
project(${PROJ_NAME})

# Find PUGIXML Library
set(PUGIXML_ROOT $ENV{PUGIXML_ROOT})
find_library(PUGIXML_LIBRARIES pugixml "${PUGIXML_ROOT}/lib")

if(NOT PUGIXML_LIBRARIES)
    message(STATUS "Cannot find pugixml cannot build ${PROJ_NAME}")
    if(NOT PUGIXML_ROOT)
        message(STATUS "PUGIXML_ROOT Env not set?")
    endif(NOT PUGIXML_ROOT)
    return()
endif()

add_library(${EXECUTION_PARSER_LIB} STATIC "")
target_compile_features(${EXECUTION_PARSER_LIB} PRIVATE cxx_std_11)
target_sources(${EXECUTION_PARSER_LIB} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/assembly.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/assembly.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/protobufmodelparser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/protobufmodelparser.h
        )
target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${PUGIXML_ROOT}/include)
target_link_libraries(${EXECUTION_PARSER_LIB} PRIVATE ${PUGIXML_LIBRARIES})
target_link_libraries(${EXECUTION_PARSER_LIB} PUBLIC re_common_proto_control)

add_executable(${EXECUTION_PARSER} "")
target_compile_features(${EXECUTION_PARSER} PRIVATE cxx_std_11)
target_link_libraries(${EXECUTION_PARSER} PRIVATE ${EXECUTION_PARSER_LIB})
target_sources(${EXECUTION_PARSER} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )