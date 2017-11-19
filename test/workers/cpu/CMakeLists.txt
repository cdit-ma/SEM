#Set our project names
set(PROJ_NAME test_cpuworker)
project(${PROJ_NAME})

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")


SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpuworkertest.cpp
    ${GOOGLE_TEST_DIR}/src/gtest_main.cc
)

add_executable(${PROJ_NAME} ${SOURCE})


target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} cpu_worker)
target_link_libraries(${PROJ_NAME} gtest)