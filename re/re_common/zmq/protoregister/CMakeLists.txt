set(PROJ_NAME zmq_protoregister)
project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
find_package(cppzmq REQUIRED)

add_library(${PROJ_NAME} STATIC "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)

target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/protoregister.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/protoregister.hpp
        )
target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC cppzmq)
