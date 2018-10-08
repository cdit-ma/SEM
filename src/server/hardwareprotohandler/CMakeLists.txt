set(PROJ_NAME hardwareprotohandler)
project (${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/hardwareprotohandler.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/hardwareprotohandler.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_systemstatus)
