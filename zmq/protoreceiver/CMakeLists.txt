set(PROJ_NAME zmq_protoreceiver)
project (${PROJ_NAME})

add_library(${PROJ_NAME} STATIC "")
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_sources(${PROJ_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.h
		)

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoregister)