add_subdirectory(protocols)
if (BUILD_TEST)
    add_subdirectory(test)
endif (BUILD_TEST)

find_package(QPID)

if (NOT QPID_FOUND)
    message(FATAL_ERROR "Cannot find QPID cannot build project")
    return()
endif ()

add_library(re_network INTERFACE)
target_compile_features(re_network INTERFACE cxx_std_11)

target_sources(re_network INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/publisher.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/subscriber.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/requester.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/replier.hpp
        )
target_include_directories(re_network
        INTERFACE
        ${QPID_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_link_libraries(re_network
        INTERFACE
        ${QPID_LIBRARIES}
        sem_types_proto
        sem_types
        )
