set(EXECUTION_PARSER "re_execution_parser")
set(EXECUTION_PARSER_LIB ${EXECUTION_PARSER}_lib)

set(PROJ_NAME ${EXECUTION_PARSER})
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

add_library(${EXECUTION_PARSER_LIB} STATIC ${SOURCE} ${HEADERS})
add_executable(${EXECUTION_PARSER} "${CMAKE_CURRENT_SOURCE_DIR}/executionparser.cpp")

target_include_directories(${EXECUTION_PARSER_LIB} PRIVATE ${PUGIXML_ROOT}/include)

# Remove the need for _lib extensions
set_target_properties(${EXECUTION_PARSER_LIB} PROPERTIES OUTPUT_NAME ${EXECUTION_PARSER})

target_link_libraries(${EXECUTION_PARSER_LIB} ${PUGIXML_LIBRARIES})
target_link_libraries(${EXECUTION_PARSER} ${EXECUTION_PARSER_LIB})