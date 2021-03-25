
find_package(Boost REQUIRED COMPONENTS system regex)

find_package(protobuf)

add_subdirectory(proto)

add_library(sem_netdata_adapter STATIC)

target_compile_features(sem_netdata_adapter
        PUBLIC
        cxx_std_17
        )

target_sources(sem_netdata_adapter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_receiver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/chart_listener.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/data_stream.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/chart_json_parser.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/device_sample_aggregator.hpp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_stream_listener.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_receiver.cpp
        )

target_include_directories(sem_netdata_adapter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(sem_netdata_adapter
        PUBLIC
        Boost::boost
        Boost::system
        Boost::regex
        Boost::dynamic_linking
        protobuf::libprotobuf
        sem_netdata_proto
        re_common_proto_systemevent
        )

if (BUILD_TEST)
    add_subdirectory(test)
endif()