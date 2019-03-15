#Set our project names
set(PROJ_NAME test_protopubsub)
project(${PROJ_NAME})


SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/prototest.cpp
    ${RE_TEST_SOURCE_DIR}/core/test_main.cpp
)

add_executable(${PROJ_NAME} ${SOURCE})

target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif(UNIX)

target_link_libraries(${PROJ_NAME} PRIVATE zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_protowriter)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)