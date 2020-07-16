add_library(zmq_protoregister)
target_compile_features(zmq_protoregister PRIVATE cxx_std_11)
set_target_properties(zmq_protoregister PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(zmq_protoregister PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/protoregister.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/protoregister.hpp
        )
target_include_directories(zmq_protoregister
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_link_libraries(zmq_protoregister
        PUBLIC
        cppzmq
        protobuf::libprotobuf
        )
