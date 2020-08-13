# TODO: Reevaluate use of SHARED here, changed from STATIC to SHARED to get build working reliably on WSLUbuntu and
#        ArchLinux
add_library(re_common_proto_control STATIC)
cdit_ma_generate_protobuf_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/controlmessage.proto)

target_compile_features(re_common_proto_control PRIVATE cxx_std_11)
set_target_properties(re_common_proto_control PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(re_common_proto_control
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/helper.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/helper.cpp
        ${PROTO_HDRS}
        ${PROTO_SRCS}
        )

target_include_directories(re_common_proto_control
        PUBLIC
        # Add generated proto headers to our include exports
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_link_libraries(re_common_proto_control
        PUBLIC
        protobuf::libprotobuf
        )