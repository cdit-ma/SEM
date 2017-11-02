cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest")
set(GOOGLE_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest)
include_directories(${GOOGLE_TEST_DIR}/include)

SET(PERIODICTEST_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventtest.cpp
)

#Set our project names
set(PROJ_NAME re_test)
project(${PROJ_NAME})

add_executable(periodicevent_test ${PERIODICTEST_SOURCE})
target_link_libraries(periodicevent_test re_core)
target_link_libraries(periodicevent_test gtest)


