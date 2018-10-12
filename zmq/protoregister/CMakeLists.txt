set(PROJ_NAME zmq_protoregister)
project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protoregister.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protoregister.hpp
)

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Compile Definitions
target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${ZMQ_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC ${ZMQ_LIBRARIES})