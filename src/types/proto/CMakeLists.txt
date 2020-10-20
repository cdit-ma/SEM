
add_library(sem_types_proto INTERFACE)
target_sources(sem_types_proto
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/proto_serializable.hpp
        )
target_include_directories(sem_types_proto INTERFACE .)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()
