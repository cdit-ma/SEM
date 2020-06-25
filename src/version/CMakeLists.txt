
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sem_version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/sem_version.hpp @ONLY)
add_library(sem_version INTERFACE)
target_compile_features(sem_version INTERFACE cxx_std_17)
target_sources(sem_version
        INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/sem_version.hpp
        )
target_include_directories(sem_version INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
