set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test/gtest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test/gtest)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test/gtest)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test/gtest)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test/gtest)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test/gtest)

add_executable(test_re_types)
target_compile_features(test_re_types PRIVATE cxx_std_17)

target_sources(test_re_types
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/uuid.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/socketaddress.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ipv4.cpp
        )

target_link_libraries(test_re_types
        PRIVATE
        re_types
        gtest
        gtest_main
        )
