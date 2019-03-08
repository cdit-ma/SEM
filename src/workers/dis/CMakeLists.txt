set(PROJ_NAME dis_worker)
project(${PROJ_NAME})

set(DIS_VERSION 7)
add_subdirectory("kdis")
set(KDIS_LIB "kdis")

add_subdirectory("tester")


message(STATUS "KDIS_LIB: ${KDIS_LIB}")
# If kdis doesn't exist on this machine we can bail out
if(NOT KDIS_LIB)
    message(STATUS "Not Found KDIS - Not building kdis_worker")
    return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker_impl.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dis_worker_impl.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/kdis)
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC re_loggers)
target_link_libraries(${PROJ_NAME} PUBLIC ${KDIS_LIB})

