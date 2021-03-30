find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

# Executable
add_executable(sem_environment_controller)
target_compile_features(sem_environment_controller PRIVATE cxx_std_11)
target_compile_definitions(sem_environment_controller PRIVATE BOOST_ALL_DYN_LINK)
set_target_properties(sem_environment_controller PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(sem_environment_controller
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/environment_controller.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/epm_output_control.h
        ${CMAKE_CURRENT_SOURCE_DIR}/epm_output_listener.h
        ${CMAKE_CURRENT_SOURCE_DIR}/epm_output_listener.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/epm_output_control.cpp
        )

target_link_libraries(sem_environment_controller
        PUBLIC
        environment_control_service
        epm_output_control_service
        epm_output_service
        re_graphml_parser_lib
        PRIVATE
        Boost::program_options
        sem_types
        sem_grpc_util
        )
