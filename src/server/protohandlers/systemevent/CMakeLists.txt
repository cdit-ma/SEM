set(PROJ_NAME logan_protohandler_systemevent)
project (${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protohandler.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protohandler.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_systemevent)
target_link_libraries(${PROJ_NAME} PUBLIC logan_server_lib)

