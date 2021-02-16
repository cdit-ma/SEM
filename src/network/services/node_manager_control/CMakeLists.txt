CDIT_MA_GENERATE_GRPC_CPP(GRPC_SRCS GRPC_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/node_manager_control_service.proto)

# Code generated at build time, that has the GENERATED cmake property, MUST!!!!!
#  be added to a target in "add_library" or "add_executable" call, not "target_sources".
add_library(node_manager_control_service STATIC
        ${GRPC_HDRS}
        ${GRPC_SRCS}
        )
set_target_properties(node_manager_control_service PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(node_manager_control_service PRIVATE cxx_std_17)

# TODO: Investigate why this needs to be public for a successful build
target_link_libraries(node_manager_control_service
        PUBLIC
        grpc++
        )
target_include_directories(node_manager_control_service
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )
