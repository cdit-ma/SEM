add_subdirectory(protocols)
if (BUILD_TEST)
    add_subdirectory(test)
endif (BUILD_TEST)

project(re_network
        VERSION ${PROJECT_VERSION})

find_package(QPID)

if (NOT QPID_FOUND)
    message(STATUS "Cannot find QPID cannot build project ${PROJ_NAME}")
    return()
endif ()

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/publisher.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/subscriber.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/requester.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/replier.hpp
        )
target_include_directories(${PROJECT_NAME} INTERFACE ${QPID_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE ${QPID_LIBRARIES})
