
# DockerClient
message(STATUS "Fetching DockerClient")
set(SKIP_BUILD_TEST ON)
FetchContent_Declare(dockerclient
        GIT_REPOSITORY https://github.com/cdit-ma/DockerClient
        # TODO: Point at release tag, this is currently pointing at master as of 2020-08-28
        GIT_TAG 2b5c14d91bcb8b8851433b7806139fdf3d99c1f2
        GIT_SHALLOW TRUE
        )
if (NOT dockerclient_POPULATE)
    FetchContent_Populate(dockerclient)
    add_subdirectory(${dockerclient_SOURCE_DIR} ${dockerclient_BINARY_DIR})
endif ()
