if (BUILD_TEST)
    # gtest
    message(STATUS "Fetching gTest")
    include(CTest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "")
    FetchContent_Declare(gtest)
    if (NOT gtest_POPULATE)
        FetchContent_Populate(gtest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG release-1.10.0
                GIT_SHALLOW TRUE
                )
        add_subdirectory(${gtest_SOURCE_DIR} ${gtest_BINARY_DIR})
    endif ()
    enable_testing()
endif ()
