add_library(grpc_util STATIC)
target_sources(grpc_util
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_util/grpc_util.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_util/grpc_util.cpp
        )

target_include_directories(grpc_util
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_compile_features(grpc_util PUBLIC cxx_std_17)
target_link_libraries(grpc_util
        PUBLIC
        sem_types
        )