#Set our project names
set(PROJ_NAME test_periodicevent)
project(${PROJ_NAME})

include_directories(${GOOGLE_TEST_DIR}/include)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventtest.cpp
)


add_executable(${PROJ_NAME} ${SOURCE})
target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)