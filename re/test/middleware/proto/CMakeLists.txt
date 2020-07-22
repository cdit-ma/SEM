set(PROJ_NAME lib_test_basic_proto)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

#Generate out protobuf for control
cdit_ma_generate_protobuf_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/basic.proto)

add_library(${PROJ_NAME})
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PUBLIC pthread)
endif()

target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})
