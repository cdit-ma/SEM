project(re_types_proto VERSION ${PROJECT_VERSION})

# Currently a head only library
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/serializable.hpp)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_TEST)
    add_subdirectory(test)
endif ()
