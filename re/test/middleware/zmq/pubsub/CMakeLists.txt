set(PROJ_NAME test_zmq_pubsub)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
)

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_helper)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_ports)
target_link_libraries(${PROJ_NAME} PRIVATE lib_test_basic_proto)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)
target_link_libraries(${PROJ_NAME} PRIVATE re_node_manager_lib)