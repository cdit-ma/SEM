#Set our project names
project(node_manager_control_protocol VERSION ${PROJECT_VERSION})

cdit_ma_generate_protobuf_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagercontrol.proto)

add_library(${PROJECT_NAME} STATIC "")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(${PROJECT_NAME} PUBLIC PROTOBUF_USE_DLLS)

target_sources(${PROJECT_NAME} PRIVATE
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROTOBUF_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

if (MSVC)
    #Windows requires protobuf in DLLs
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
    #Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

# Link Libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROTOBUF_LIBRARIES})