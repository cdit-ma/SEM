cdit_ma_generate_protobuf_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/experimentdefinition.proto)

add_library(experiment_definition_protocol)
target_compile_features(experiment_definition_protocol PRIVATE cxx_std_14)
set_target_properties(experiment_definition_protocol PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(experiment_definition_protocol PRIVATE
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

target_include_directories(experiment_definition_protocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link Libraries
target_link_libraries(experiment_definition_protocol 
        PUBLIC 
        protobuf::libprotobuf
        )