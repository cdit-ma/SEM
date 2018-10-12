set(PROJ_NAME zmq_protoreplier)
project (${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protoreplier.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protoreplier.hpp
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoregister)
