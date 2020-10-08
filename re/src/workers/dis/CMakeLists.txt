set(DIS_VERSION 7)
set(KDIS_LIB "kdis")

find_package(KDIS)

# If kdis doesn't exist on this machine we can bail out
if(NOT KDIS_FOUND)
    message(STATUS "Not Found KDIS - Not building kdis_worker")
    return()
endif()

add_library(dis_worker SHARED)
target_compile_features(dis_worker PRIVATE cxx_std_11)
set_target_properties(dis_worker PROPERTIES POSITION_INDEPENDENT_CODE ON)
# Suppress dynamic exception and auto_ptr deprecation warnings
target_compile_options(dis_worker PRIVATE -Wno-deprecated -Wno-deprecated-declarations)

target_sources(dis_worker
    PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker_impl.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker_impl.h
)

target_include_directories(dis_worker
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_link_libraries(dis_worker
    PUBLIC 
    re_core
    re_loggers
    kdis
    )


export(TARGETS
        dis_worker
        NAMESPACE sem::
        APPEND
        FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sem_exports.cmake
        EXPORT_LINK_INTERFACE_LIBRARIES
        )