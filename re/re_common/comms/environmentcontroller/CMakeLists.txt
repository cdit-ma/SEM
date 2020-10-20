find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

# Library
add_library(re_environment_controller_lib STATIC)
target_compile_features(re_environment_controller_lib PRIVATE cxx_std_11)
set_target_properties(re_environment_controller_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(re_environment_controller_lib
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/environmentcontroller.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/environmentcontroller.h
        )
target_link_libraries(re_environment_controller_lib
        PUBLIC
        re_common_proto_environmentcontrol
        zmq_protorequester
        re_graphml_parser_lib
        sem_types
        re_common_proto_control
        experiment_definition_protocol
        experiment_definition
        re_graphml_parser_lib
        )
target_include_directories(re_environment_controller_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

# Executable
add_executable(re_environment_controller)
target_compile_features(re_environment_controller PRIVATE cxx_std_11)
target_compile_definitions(re_environment_controller PRIVATE BOOST_ALL_DYN_LINK)
set_target_properties(re_environment_controller PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(re_environment_controller
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

target_link_libraries(re_environment_controller
        PUBLIC
        re_environment_controller_lib
        PRIVATE
        Boost::program_options
        )
