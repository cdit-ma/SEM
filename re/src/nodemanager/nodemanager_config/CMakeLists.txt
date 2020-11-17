find_package(Boost 1.30.0 COMPONENTS program_options filesystem REQUIRED)

# Node manager library
add_library(nodemanager_config)
target_compile_features(nodemanager_config PRIVATE cxx_std_17)
target_compile_definitions(nodemanager_config PUBLIC BOOST_ALL_DYN_LINK)

target_sources(nodemanager_config
        PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerconfig.cpp
        # Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerconfig.h
        )
target_include_directories(nodemanager_config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(nodemanager_config
        PUBLIC
        sem_types
        PRIVATE
        Boost::filesystem
        Boost::program_options
        )
