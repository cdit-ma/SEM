set(PROJ_NAME dis_worker)
project(${PROJ_NAME})


# Try and find an OpenCL Implementation
find_library(KDIS_LIB kdis)


# If OpenCL doesn't exist on this machine we can bail out
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

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC re_loggers)
target_link_libraries(${PROJ_NAME} PUBLIC ${KDIS_LIB})

