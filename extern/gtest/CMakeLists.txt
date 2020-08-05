if (BUILD_TEST)
    # gtest
    message(STATUS "Fetching gTest")
    #include(CTest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "")
    FetchContent_Declare(googletest
    )
    if (NOT googletest_POPULATE)
        FetchContent_Populate(googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG release-1.10.0
                GIT_SHALLOW TRUE
                )
        add_subdirectory(${googletest_SOURCE_DIR}/googlemock ${googletest_BINARY_DIR})
    endif ()
    #enable_testing()
endif ()
