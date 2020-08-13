find_package(Boost 1.30.0 COMPONENTS filesystem system REQUIRED)

add_library(zmq_protowriter)
target_compile_features(zmq_protowriter PRIVATE cxx_std_11)

target_compile_definitions(zmq_protowriter
        PUBLIC
        BOOST_ALL_DYN_LINK
        )

set_target_properties(zmq_protowriter PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(zmq_protowriter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/monitor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/protowriter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cachedprotowriter.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/protowriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cachedprotowriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/monitor.cpp
        )
target_include_directories(zmq_protowriter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(zmq_protowriter
        PUBLIC
        cppzmq
        protobuf::libprotobuf
        PRIVATE
        Boost::system
        Boost::filesystem
        )
