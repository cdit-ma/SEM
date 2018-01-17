#Set our project names
set(PROJ_NAME test_openclworker)
project(${PROJ_NAME})

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanagertest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworkertest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common.cpp
    ${GOOGLE_TEST_DIR}/src/gtest_main.cc
)

add_executable(${PROJ_NAME} ${SOURCE})

target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
endif(UNIX)

target_link_libraries(${PROJ_NAME} opencl_worker)
target_link_libraries(${PROJ_NAME} gtest)