set(PROJ_NAME utility_worker)
project(${PROJ_NAME})


find_package(Boost 1.30.0 REQUIRED)

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
if (MSVC)
    target_compile_options(${PROJ_NAME} PRIVATE "/bigobj")
endif(MSVC)

target_sources(${PROJ_NAME} PRIVATE
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
target_include_directories(${PROJ_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} re_loggers)
