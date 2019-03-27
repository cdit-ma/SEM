set(PROJ_NAME zmq_protorequester)
project (${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protorequester.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protorequester.hpp
    )

add_library(${PROJ_NAME} STATIC "")
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_sources(${PROJ_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/protorequester.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/protorequester.hpp
		)

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoregister)
