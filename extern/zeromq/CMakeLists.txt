
# ZeroMQ
message(STATUS "Fetching ZeroMQ")
set(BUILD_TESTS OFF CACHE BOOL "")
set(ENABLE_DRAFTS OFF CACHE BOOL "")
set(WITH_PERF_TOOL OFF CACHE BOOL "")
FetchContent_Declare(zeromq
        GIT_REPOSITORY https://github.com/zeromq/libzmq.git
        GIT_TAG v4.3.0
        GIT_SHALLOW TRUE
        )
if (NOT zeromq_POPULATE)
    FetchContent_Populate(zeromq)
    add_subdirectory(${zeromq_SOURCE_DIR} ${zeromq_BINARY_DIR})
endif ()

# cppzmq
message(STATUS "Fetching cppzmq")
set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "")
cmake_policy(SET CMP0077 NEW)
FetchContent_Declare(cppzmq
        GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
        GIT_TAG v4.7.1
        )
if (NOT cppzmq_POPULATE)
    FetchContent_Populate(cppzmq)
    add_subdirectory(${cppzmq_SOURCE_DIR} ${cppzmq_BINARY_DIR})
endif ()
