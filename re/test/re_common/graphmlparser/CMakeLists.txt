#Set our project names
set(PROJ_NAME test_graphmlparser)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

# See https://github.com/vector-of-bool/cmrc
include(${CMAKE_MODULE_PATH}/CMakeRC.cmake)
cmrc_add_resource_library(
        test_models
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


add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
)

target_link_libraries(${PROJ_NAME} PRIVATE re_graphml_parser_lib)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJ_NAME} PRIVATE test_models)