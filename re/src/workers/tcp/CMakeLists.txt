# See the TCP Worker wiki page for the reasoning.
if (APPLE)
    message(WARNING "Not building TCP Worker on apple platforms due to bug that causes hangs")
    return()
endif(APPLE)

find_package(Boost 1.67.0 REQUIRED COMPONENTS thread regex)

add_library(tcp_worker)
target_compile_features(tcp_worker PRIVATE cxx_std_17)

target_sources(tcp_worker PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_worker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tcpworkerimpl.cpp
        # Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_worker.h
        ${CMAKE_CURRENT_SOURCE_DIR}/tcpworkerimpl.h
        )

target_include_directories(tcp_worker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tcp_worker 
        PUBLIC
        re_core
        re_loggers
        Boost::dynamic_linking
        Boost::boost
        Boost::thread
        Boost::regex
        )

export(TARGETS
        tcp_worker
        NAMESPACE sem::
        FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sem_exports.cmake
        EXPORT_LINK_INTERFACE_LIBRARIES
        )