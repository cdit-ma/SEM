CDIT_MA_GENERATE_GRPC_CPP(GRPC_SRCS GRPC_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/node_manager_registration_service.proto)
add_library(node_manager_registration_service STATIC)
message(WARNING ${GRPC_HDRS} ${GRPC_SRCS})
target_sources(node_manager_registration_service
        PUBLIC
        ${GRPC_HDRS}
        PRIVATE
        ${GRPC_SRCS}
        )

# TODO: Investigate why this needs to be public for a successful build
target_link_libraries(node_manager_registration_service
        PUBLIC
        grpc++
        )
target_include_directories(node_manager_registration_service
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )
