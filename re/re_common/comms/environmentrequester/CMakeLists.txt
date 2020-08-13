add_library(zmq_environmentrequester STATIC)
target_compile_features(zmq_environmentrequester PRIVATE cxx_std_11)
set_target_properties(zmq_environmentrequester PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(zmq_environmentrequester
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.h
        ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/heartbeater.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/heartbeater.h
        )

target_include_directories(zmq_environmentrequester PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zmq_environmentrequester 
        PUBLIC
        re_common_proto_control
        zmq_protorequester
        )