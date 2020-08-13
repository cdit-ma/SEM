add_executable(test_primitivetranslations)
target_compile_features(test_primitivetranslations PRIVATE cxx_std_11)

target_sources(test_primitivetranslations
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
        )

target_include_directories(test_primitivetranslations
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_link_libraries(test_primitivetranslations
        PRIVATE
        gtest_main
        re_core
        )
