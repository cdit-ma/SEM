project(re_types_proto VERSION ${PROJECT_VERSION})

add_library(${PROJECT_NAME} STATIC "")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_sources(${PROJECT_NAME} PRIVATE
        serializable.hpp)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()
