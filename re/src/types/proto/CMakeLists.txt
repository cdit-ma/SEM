
add_library(re_types_proto INTERFACE)
target_sources(re_types_proto
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/proto_serializable.hpp
        )
target_include_directories(re_types_proto INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_TEST)
    add_subdirectory(test)
endif ()
