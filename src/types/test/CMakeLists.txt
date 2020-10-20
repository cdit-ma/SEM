if (MSVC)
    set(TEST_OUTPUT_DIR "")
else ()
    set(TEST_OUTPUT_DIR "/test/gtest")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})

add_executable(test_sem_types)
target_compile_features(test_sem_types PRIVATE cxx_std_17)

target_sources(test_sem_types
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/uuid.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/socketaddress.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ipv4.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/result.cpp
        )

target_link_libraries(test_sem_types
        PRIVATE
        sem_types
        gtest
        gtest_main
        # Include boost headers for uuid.hpp and uuid_generators.hpp
        Boost::boost
        )
