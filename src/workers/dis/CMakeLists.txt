set(PROJ_NAME dis_worker)
project(${PROJ_NAME}
        VERSION ${PROJECT_VERSION})

set(DIS_VERSION 7)
set(KDIS_LIB "kdis")

find_package(KDIS)

# If kdis doesn't exist on this machine we can bail out
if(NOT KDIS_FOUND)
    message(STATUS "Not Found KDIS - Not building kdis_worker")
    return()
endif()

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
# Suppress dynamic exception and auto_ptr deprecation warnings
target_compile_options(${PROJ_NAME} PRIVATE -Wno-deprecated -Wno-deprecated-declarations)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker_impl.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker_impl.h
)
target_include_directories(${PROJ_NAME} PUBLIC ${KDIS_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC re_loggers)
target_link_libraries(${PROJ_NAME} PUBLIC ${KDIS_LIBRARIES})
