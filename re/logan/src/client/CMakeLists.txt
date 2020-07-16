set(PROJ_NAME logan_client)
project(logan_client VERSION ${PROJECT_VERSION})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

# Library
add_library(logan_client_lib STATIC)
target_compile_features(logan_client_lib PRIVATE cxx_std_11)

target_sources(logan_client_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/logcontroller.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/logcontroller.cpp
        )
target_include_directories(logan_client_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(logan_client_lib
        PUBLIC
        re_common_proto_systemevent
        re_common_proto_control
        zmq_protowriter
        system_info_handler
        re_common_proto_systemevent
        protobuf::libprotobuf
        )

if (UNIX)
    target_link_libraries(logan_client_lib PUBLIC pthread)
endif (UNIX)


# Executable
add_executable(logan_client)
target_compile_features(logan_client PRIVATE cxx_std_11)

target_sources(logan_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )
target_link_libraries(logan_client
        PRIVATE
        logan_client_lib
        Boost::program_options
        sem_version
        execution_helper
        )
