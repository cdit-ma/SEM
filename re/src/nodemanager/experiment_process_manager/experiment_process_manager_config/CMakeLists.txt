find_package(Boost 1.67.0 COMPONENTS program_options REQUIRED)

add_library(experiment_process_manager_config)
target_compile_features(experiment_process_manager_config PRIVATE cxx_std_17)

target_sources(experiment_process_manager_config
        PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/experimentprocessmanagerconfig.cpp
        # Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/experimentprocessmanagerconfig.h
        )
target_include_directories(experiment_process_manager_config
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(experiment_process_manager_config
        PUBLIC
        sem_types
        Boost::program_options
        Boost::dynamic_linking
        PRIVATE
        )
