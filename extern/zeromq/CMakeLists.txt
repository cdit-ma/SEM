
# ZeroMQ
message(STATUS "Fetching ZeroMQ")
FetchContent_Declare(
        zeromq
        GIT_REPOSITORY https://github.com/zeromq/libzmq.git
        GIT_TAG v4.3.0
)
if (NOT zeromq_POPULATE)
    FetchContent_Populate(zeromq)
    add_subdirectory(${zeromq_SOURCE_DIR} ${zeromq_BINARY_DIR})
endif ()

# cppzmq
message(STATUS "Fetching cppzmq")
message(WARNING "Pulling cppzmq from HEAD. Consider pinning to version/tag once cppzmq release version > 4.6.0")
set(CPPZMQ_BUILD_TESTS OFF)
cmake_policy(SET CMP0077 NEW)
FetchContent_Declare(
        cppzmq
        GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
        GIT_TAG master
)
if (NOT cppzmq_POPULATE)
    FetchContent_Populate(cppzmq)
    add_subdirectory(${cppzmq_SOURCE_DIR} ${cppzmq_BINARY_DIR})
endif ()
