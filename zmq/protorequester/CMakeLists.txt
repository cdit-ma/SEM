set(PROJ_NAME zmq_protorequester)
project (${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protorequester.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protorequester.hpp
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoregister)
