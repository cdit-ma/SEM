set(PROJ_NAME ospl_dds_helper)
project(${PROJ_NAME}
        VERSION ${PROJECT_VERSION})

find_package(OSPL_DDS)

if(NOT OSPL_DDS_FOUND)
    message(STATUS "Cannot find OSPL_DDS cannot build project ${PROJ_NAME}")
    return()
endif()

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.h
)
target_include_directories(${PROJ_NAME} PUBLIC ${OSPL_DDS_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} PUBLIC ${OSPL_DDS_LIBRARIES})
