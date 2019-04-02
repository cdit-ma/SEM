set(PROJ_NAME memory_worker)
project(${PROJ_NAME})

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker_impl.h
)
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
