# gtest
message(STATUS "Fetching gTest")

# Googletest doesn't work with sccache; they force /Zi
if (MSVC AND CMAKE_CXX_COMPILER_LAUNCHER STREQUAL "sccache")
    unset(CMAKE_CXX_COMPILER_LAUNCHER)
    unset(CMAKE_C_COMPILER_LAUNCHER)
endif ()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.10.0
        GIT_SHALLOW TRUE
        )
FetchContent_GetProperties(googletest)
if (NOT googletest_POPULATE)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif ()
