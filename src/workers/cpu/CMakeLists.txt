set(PROJ_NAME cpu_worker)
project(${PROJ_NAME})

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
# Disable Optimizing
if (UNIX)
    SET(NO_OPTIMIZE_FLAG "-O0")
elseif(MSVC)
    SET(NO_OPTIMIZE_FLAG "-Od /wd4996 /wd4244")
endif(UNIX)
target_compile_options(${PROJ_NAME} PRIVATE ${NO_OPTIMIZE_FLAG})

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/floatop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/whets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry_1.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry.h
)
target_link_libraries(${PROJ_NAME} PUBLIC re_core)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/profiler")
