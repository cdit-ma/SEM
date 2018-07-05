#Set our project names
set(PROJ_NAME test_primitivetranslations)
project(${PROJ_NAME})


SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/primitivetranslations.cpp
    ${RE_TEST_SOURCE_DIR}/core/test_main.cpp
)


add_executable(${PROJ_NAME} ${SOURCE})

target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
endif(UNIX)

target_link_libraries(${PROJ_NAME} gtest)