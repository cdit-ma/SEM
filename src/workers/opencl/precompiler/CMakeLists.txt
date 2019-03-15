set(PROJ_NAME precompile_opencl)
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS filesystem REQUIRED)


add_executable(${PROJ_NAME} "")

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiler.cpp
)

target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIR})
# Link Libraries
target_link_libraries(${PROJ_NAME} PRIVATE opencl_worker)