find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(aggregation_requester)
target_sources(aggregation_requester
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )
add_library(aggregation_requester_lib STATIC)
target_sources(aggregation_requester_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/aggregationrequester.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/aggregationrequester.cpp
        )

target_include_directories(aggregation_requester_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_definitions(aggregation_requester PRIVATE -DBOOST_ALL_DYN_LINK)

target_link_libraries(aggregation_requester_lib
        PUBLIC
        re_common_proto_aggregation_message
        zmq_protorequester
        )

target_link_libraries(aggregation_requester
        PRIVATE
        aggregation_requester_lib
        Boost::program_options
        )
