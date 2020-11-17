find_package(Boost 1.30.0 COMPONENTS program_options filesystem REQUIRED)

# Node manager library
add_library(epm_registry)
target_compile_features(epm_registry PRIVATE cxx_std_17)
target_compile_definitions(epm_registry PUBLIC BOOST_ALL_DYN_LINK)

target_sources(epm_registry
        PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistryimpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistrarimpl.cpp
        # Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistry.h
        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistryimpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/epmregistrarimpl.h
        )
target_include_directories(epm_registry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(epm_registry
        PUBLIC
        sem_types
        sem_grpc_util
        epm_registration_service
        nodemanager_config
        PRIVATE
        Boost::filesystem
        Boost::program_options
        )
