add_subdirectory(experiment_process_manager_config)
find_package(Boost 1.30.0 COMPONENTS program_options filesystem REQUIRED)

if (false)

add_library(experiment_process_manager_lib)
target_compile_features(experiment_process_manager_lib PRIVATE cxx_std_17)
target_compile_definitions(experiment_process_manager_lib PUBLIC BOOST_ALL_DYN_LINK)

target_sources(experiment_process_manager_lib
        PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/experimentprocessmanager.cpp
        # Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/experimentprocessmanager.h
        )
target_include_directories(experiment_process_manager_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(experiment_process_manager_lib
        PUBLIC
        sem_types
        sem_grpc_util
        PRIVATE
        )
endif (false)

add_executable(experiment_process_manager)
target_compile_features(experiment_process_manager PRIVATE cxx_std_17)
target_compile_definitions(experiment_process_manager PUBLIC BOOST_ALL_DYN_LINK)

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