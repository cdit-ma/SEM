set(PROJ_NAME zmq_environmentrequester)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/heartbeater.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_control)
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protorequester)