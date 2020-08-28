
# DockerClient
message(STATUS "Fetching DockerClient")
set(SKIP_BUILD_TEST ON)
FetchContent_Declare(dockerclient
        GIT_REPOSITORY https://github.com/cdit-ma/DockerClient
        GIT_TAG master
        GIT_SHALLOW TRUE
        )
if (NOT dockerclient_POPULATE)
    FetchContent_Populate(dockerclient)
    add_subdirectory(${dockerclient_SOURCE_DIR} ${dockerclient_BINARY_DIR})
endif ()
