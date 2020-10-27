find_package(Boost 1.30.0 COMPONENTS program_options filesystem REQUIRED)

# Node manager library
add_library(re_node_manager_lib SHARED)
target_compile_features(re_node_manager_lib PRIVATE cxx_std_17)
target_compile_definitions(re_node_manager_lib PUBLIC BOOST_ALL_DYN_LINK)

target_sources(re_node_manager_lib PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerconfig.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagercontrolimpl.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistryimpl.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistrarimpl.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/trigger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/strategy.cpp


        # Headers

        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerconfig.h
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagercontrolimpl.h

        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistry.h
        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistryimpl.h

        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistrarimpl.h

        ${CMAKE_CURRENT_SOURCE_DIR}/trigger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/strategy.h
        )
target_include_directories(re_node_manager_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(re_node_manager_lib
        PUBLIC
        re_loggers
        re_core
        sem_types
        sem_grpc_util
        node_manager_registration_service
        node_manager_control_service
        epm_registration_service
        system_info_broker
        PRIVATE
        Boost::filesystem
        Boost::program_options
        )

# Link against pthread if unix
if (UNIX)
    target_link_libraries(re_node_manager_lib PRIVATE pthread)
    target_link_libraries(re_node_manager_lib PRIVATE dl)
endif (UNIX)

# Node manager executable
add_executable(re_node_manager)
target_compile_features(re_node_manager PRIVATE cxx_std_17)

target_sources(re_node_manager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagermain.cpp
        )

target_link_libraries(re_node_manager
        PUBLIC
        re_node_manager_lib
        )

# Experiment manager executable
add_executable(experiment_process_manager)
target_compile_features(experiment_process_manager PRIVATE cxx_std_17)

target_sources(experiment_process_manager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/experimentprocessmanagermain.cpp
        )

target_link_libraries(experiment_process_manager
        PUBLIC
        re_node_manager_lib
        )


if (BUILD_TEST)
    add_subdirectory(test)
endif ()