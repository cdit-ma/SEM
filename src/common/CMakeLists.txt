
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sem_version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/sem_version.hpp @ONLY)
add_library(sem_version STATIC)
target_compile_features(sem_version PUBLIC cxx_std_17)
target_sources(sem_version
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/sem_version.hpp
        )
# CMake doesn't recognise a single hpp file as c++ for some reason, manually set linker language.
set_target_properties(sem_version PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(sem_version PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
