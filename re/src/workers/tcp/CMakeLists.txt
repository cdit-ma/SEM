set(PROJ_NAME tcp_worker)
project(${PROJ_NAME}
        VERSION ${PROJECT_VERSION})

# See the TCP Worker wiki page for the reasoning.
if (APPLE)
    message(WARNING "Not building TCP Worker on apple platforms due to bug that causes hangs")
    return()
endif(APPLE)

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)

target_sources(${PROJ_NAME} PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_worker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tcpworkerimpl.cpp
        # Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_worker.h
        ${CMAKE_CURRENT_SOURCE_DIR}/tcpworkerimpl.h
        )

target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC re_loggers)
