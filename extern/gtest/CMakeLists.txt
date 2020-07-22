if (BUILD_TEST)
    # gtest
    message(STATUS "Fetching gTest")
    include(CTest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
            gtest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.10.0
    )
    if (NOT gtest_POPULATE)
        FetchContent_Populate(gtest)
        add_subdirectory(${gtest_SOURCE_DIR}/googlemock ${gtest_BINARY_DIR})
    endif ()
    enable_testing()
endif ()
