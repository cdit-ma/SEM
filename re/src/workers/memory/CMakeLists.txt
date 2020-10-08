add_library(memory_worker SHARED)
target_compile_features(memory_worker PRIVATE cxx_std_11)
set_target_properties(memory_worker PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(memory_worker PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker_impl.h
)
target_link_libraries(memory_worker PUBLIC re_core)
target_include_directories(memory_worker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

export(TARGETS
        memory_worker
        NAMESPACE sem::
        FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sem_exports.cmake
        EXPORT_LINK_INTERFACE_LIBRARIES
        )