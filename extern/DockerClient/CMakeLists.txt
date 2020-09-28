
# DockerClient
message(STATUS "Fetching DockerClient")
set(SKIP_BUILD_TEST ON)
FetchContent_Declare(dockerclient
        GIT_REPOSITORY https://github.com/cdit-ma/DockerClient
        GIT_TAG 08da051a6cac70a08ae115e5767b99d99c6ce360
        )
if (NOT dockerclient_POPULATE)
    FetchContent_Populate(dockerclient)
    add_subdirectory(${dockerclient_SOURCE_DIR} ${dockerclient_BINARY_DIR})
endif ()
