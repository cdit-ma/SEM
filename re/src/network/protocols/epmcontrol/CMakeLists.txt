#Set our project names
project(epm_control_protocol VERSION ${PROJECT_VERSION})

CDIT_MA_GENERATE_PROTOBUF_CPP(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/epmcontrol.proto)

add_library(${PROJECT_NAME} STATIC "")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${PROJECT_NAME} PRIVATE
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )


# Link Libraries
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        protobuf::libprotobuf
        )