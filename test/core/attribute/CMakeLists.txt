#Set our project names
set(PROJ_NAME test_attribute)
project(${PROJ_NAME})

add_executable(${PROJ_NAME} "")

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/attributetests.cpp
)

target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SOURCE_PATH})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif(UNIX)

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)