
# gRPC
message(STATUS "Fetching gRPC")
FetchContent_Declare(
        grpc
        GIT_REPOSITORY https://github.com/grpc/grpc.git
        GIT_TAG v1.30.2
)
if (NOT grpc_POPULATE)
    FetchContent_Populate(grpc)
    add_subdirectory(${grpc_SOURCE_DIR} ${grpc_BINARY_DIR})
    set(Protobuf_IMPORT_DIRS ${grpc_SOURCE_DIR}/third_party/protobuf/src PARENT_SCOPE)
endif ()

function(CDIT_MA_GENERATE_PROTOBUF_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: CDIT_MA_GENERATE_PROTOBUF_CPP() called without any proto files")
        return()
    endif()

    if(PROTOBUF_GENERATE_CPP_APPEND_PATH) # This variable is common for all types of output.
        # Create an include path for each file specified
        foreach(FIL ${ARGN})
            get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
            get_filename_component(ABS_PATH ${ABS_FIL} PATH)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    else()
        set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if(DEFINED Protobuf_IMPORT_DIRS)
        foreach(DIR ${Protobuf_IMPORT_DIRS})
            get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    endif()

    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)

        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h")

        add_custom_command(
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h"
                COMMAND  $<TARGET_FILE:protoc>
                ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
                ${_protobuf_include_path} ${ABS_FIL}
                DEPENDS ${ABS_FIL} $<TARGET_FILE:protoc>
                COMMENT "Running C++ protocol buffer compiler on ${FIL}"
                VERBATIM)
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()


function(CDIT_MA_GENERATE_GRPC_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: CDIT_MA_GENERATE_GRPC_CPP() called without any proto files")
        return()
    endif()

    if(PROTOBUF_GENERATE_CPP_APPEND_PATH) # This variable is common for all types of output.
        # Create an include path for each file specified
        foreach(FIL ${ARGN})
            get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
            get_filename_component(ABS_PATH ${ABS_FIL} PATH)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    else()
        set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if(DEFINED Protobuf_IMPORT_DIRS)
        foreach(DIR ${Protobuf_IMPORT_DIRS})
            get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    endif()

    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)

        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h")

        add_custom_command(
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h"
                COMMAND $<TARGET_FILE:protoc>
                ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
                --plugin=protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
                ${_protobuf_include_path} ${ABS_FIL}
                DEPENDS ${ABS_FIL} $<TARGET_FILE:protoc>
                COMMENT "Running gRPC C++ protocol buffer compiler on ${FIL}"
                VERBATIM)
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()