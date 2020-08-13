find_package(Boost 1.30.0 REQUIRED)

add_library(utility_worker SHARED)
target_compile_features(utility_worker PRIVATE cxx_std_11)
set_target_properties(utility_worker PROPERTIES POSITION_INDEPENDENT_CODE ON)
if (MSVC)
    target_compile_options(utility_worker PRIVATE "/bigobj")
endif(MSVC)

target_sources(utility_worker PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/exprtk.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/exprtkwrapper.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/exprtkwrapper.h
)
target_include_directories(utility_worker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(utility_worker
        PRIVATE
        re_core
        re_loggers
        )
