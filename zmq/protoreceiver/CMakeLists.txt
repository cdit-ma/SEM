set(PROJ_NAME zmq_protoreceiver)
project (${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoregister)