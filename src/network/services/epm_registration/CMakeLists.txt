CDIT_MA_GENERATE_GRPC_CPP(GRPC_SRCS GRPC_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/epm_registration_service.proto)

# Code generated at build time, that has the GENERATED cmake property, MUST!!!!!
#  be added to a target in "add_library" or "add_executable" call, not "target_sources".
add_library(epm_registration_service STATIC
        ${GRPC_HDRS}
        ${GRPC_SRCS}
        )
set_target_properties(epm_registration_service PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(epm_registration_service PRIVATE cxx_std_11)

# TODO: Investigate why this needs to be public for a successful build
target_link_libraries(epm_registration_service
        PUBLIC
        grpc++
        )
target_include_directories(epm_registration_service
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )
