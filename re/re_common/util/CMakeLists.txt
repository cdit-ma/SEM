add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser")

add_library(execution_helper INTERFACE)
target_sources(execution_helper INTERFACE execution.hpp)
target_include_directories(execution_helper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})