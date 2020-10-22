add_library(sem_grpc_util STATIC)
target_sources(sem_grpc_util
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_util/grpc_util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_util/server.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_util/grpc_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_util/server.cpp
        )

target_include_directories(sem_grpc_util
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_compile_features(sem_grpc_util PUBLIC cxx_std_17)
set_target_properties(sem_grpc_util PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(sem_grpc_util
        PUBLIC
        sem_types
        grpc++
        )