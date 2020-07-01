add_subdirectory(testprotomessage)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test)

add_executable(test_re_types_proto)
target_compile_features(test_re_types_proto PRIVATE cxx_std_17)

target_sources(test_re_types_proto PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_proto_serializable.cpp
        )

target_link_libraries(test_re_types_proto
        PRIVATE
        re_types
        re_types_proto
        gtest
        gtest_main
        re_types_proto_testmessage
        )