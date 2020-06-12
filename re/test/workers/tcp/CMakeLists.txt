set(PROJ_NAME test_tcpworker)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

# See TCP Worker page on the wiki for reasoning behind Apple builds
if (NOT tcp_worker)
    message(WARNING "TCP worker library not found - skipping TCP Worker tests")
    return()
endif()

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)

target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tcpworkertest.cpp
        )

target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SOURCE_PATH})

target_link_libraries(${PROJ_NAME} PRIVATE tcp_worker)
target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
