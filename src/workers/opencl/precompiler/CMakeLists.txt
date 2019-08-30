set(PROJ_NAME precompile_opencl)
project(${PROJ_NAME}
        VERSION ${PROJECT_VERSION})

find_package(Boost 1.30.0 COMPONENTS filesystem REQUIRED)

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
# Suppress ignored cl_int alignment attribute warning in opencl library
target_compile_options(${PROJ_NAME} PRIVATE -Wno-ignored-attributes)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiler.cpp
)
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} PRIVATE opencl_worker)
