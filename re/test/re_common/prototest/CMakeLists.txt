add_executable(test_protopubsub)
target_compile_features(test_protopubsub PRIVATE cxx_std_11)

target_sources(test_protopubsub
        PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
        )

target_include_directories(test_protopubsub PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(test_protopubsub
        PRIVATE
        zmq_protoreceiver
        zmq_protowriter
        gtest_main
        )