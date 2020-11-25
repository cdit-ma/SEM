add_subdirectory(experiment_process_manager_config)

add_executable(experiment_process_manager)
target_compile_features(experiment_process_manager PRIVATE cxx_std_17)

target_sources(experiment_process_manager
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/experimentprocessmanagermain.cpp
        )

target_link_libraries(experiment_process_manager
        PRIVATE
        experiment_process_manager_config
        epm_registration_service
        sem_grpc_util
        )