find_package(Boost 1.67.0 COMPONENTS program_options filesystem REQUIRED)

# Node manager library
add_library(nodemanager_config)
target_compile_features(nodemanager_config PRIVATE cxx_std_17)

target_sources(nodemanager_config
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerconfig.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerconfig.cpp
        )
target_include_directories(nodemanager_config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(nodemanager_config
        PUBLIC
        sem_types
        Boost::dynamic_linking
        PRIVATE
        Boost::filesystem
        Boost::program_options
        )
