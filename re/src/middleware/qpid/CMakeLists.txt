set(PROJ_NAME qpid_helper)
project (${PROJ_NAME}
        VERSION ${PROJECT_VERSION})

find_package(QPID)

if(NOT QPID_FOUND)
    message(STATUS "Cannot find QPID cannot build project ${PROJ_NAME}")
    return()
endif()

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qpidhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qpidhelper.h
)
target_include_directories(${PROJ_NAME} PUBLIC ${QPID_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} PUBLIC ${QPID_LIBRARIES})
