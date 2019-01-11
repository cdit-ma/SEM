#Set our project names
set(PROJ_NAME test_graphmlparser)
project(${PROJ_NAME})

# See https://github.com/vector-of-bool/cmrc
include(${CMAKE_MODULE_PATH}/CMakeRC.cmake)
cmrc_add_resource_library(test_models
        models/complex_replication.graphml
        models/complex_replication.json
        models/model.graphml
        models/RE414.graphml
        models/RE414.json
        models/RE414_uneven_depth.graphml
        models/RE414_uneven_depth.json
        models/simple_replication.graphml
        models/simple_replication.json
        )

SET(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparsertest.cpp
        ${RE_TEST_SOURCE_DIR}/core/test_main.cpp
        )

add_executable(${PROJ_NAME} ${SOURCE})

target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif(UNIX)

target_link_libraries(${PROJ_NAME} PRIVATE re_graphml_parser_lib)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE test_models)