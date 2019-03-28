set(PROJ_NAME zmq_protoreceiver)
project (${PROJ_NAME})

add_library(${PROJ_NAME} STATIC "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${PROJ_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.h
		)
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoregister)
