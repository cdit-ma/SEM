
# cuda-api-wrappers

# Bail out early if we don't have a cuda compiler installed
include(CheckLanguage)
check_language(CUDA)
if (NOT CMAKE_CUDA_COMPILER)
    return()
endif()

# Build CUDA cpp wrapper library
message(STATUS "Fetching cuda-api-wrappers")
set(SKIP_BUILD_TEST ON)
FetchContent_Declare(cuda-api-wrappers
        GIT_REPOSITORY https://github.com/eyalroz/cuda-api-wrappers.git
        # TODO: Point at release tag, this is currently pointing at an arbitrary commit as of 2020-08-28
        GIT_TAG "b33726d4cd72760fbd9d06c370e5fd5ee59c7d31"
        )
if (NOT cuda-api-wrappers_POPULATE)
    FetchContent_Populate(cuda-api-wrappers)
    add_subdirectory(${cuda-api-wrappers_SOURCE_DIR} ${cuda-api-wrappers_BINARY_DIR})
endif ()
