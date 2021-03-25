cdit_ma_generate_protobuf_cpp(PROTO_SRCS PROTO_HDRS
        ${CMAKE_CURRENT_SOURCE_DIR}/netdata_chart.proto
        )

add_library(sem_netdata_proto)

target_compile_features(sem_netdata_proto
        PRIVATE
        cxx_std_17
        )

target_sources(sem_netdata_proto
        PRIVATE
        ${PROTO_HDRS}
        ${PROTO_SRCS}
        )

target_include_directories(sem_netdata_proto
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )

target_link_libraries(sem_netdata_proto
        PUBLIC
        protobuf::libprotobuf
        )