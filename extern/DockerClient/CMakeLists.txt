
# DockerClient
message(STATUS "Fetching DockerClient")
set(SKIP_BUILD_TEST ON)
FetchContent_Declare(dockerclient
        GIT_REPOSITORY https://github.com/cdit-ma/DockerClient
        # TODO: Point at release tag, this is currently pointing at master as of 2020-08-28
        GIT_TAG 452d2bd72bf334fc7b97858e65c7d08ed04aa537
        GIT_SHALLOW TRUE
        )
if (NOT dockerclient_POPULATE)
    FetchContent_Populate(dockerclient)
    add_subdirectory(${dockerclient_SOURCE_DIR} ${dockerclient_BINARY_DIR})
endif ()
