set(PROJ_NAME precompile_opencl)
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS filesystem REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiler.cpp
    )

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} opencl_worker)