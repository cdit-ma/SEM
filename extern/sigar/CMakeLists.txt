
# SIGAR
if (NO_SIGAR)
    return()
endif ()
message(STATUS "Fetching SIGAR")
FetchContent_Declare(sigar
        GIT_REPOSITORY https://github.com/cdit-ma/sigar
        GIT_TAG master
        GIT_SHALLOW TRUE
        )
if (NOT sigar_POPULATE)
    FetchContent_Populate(sigar)
    add_subdirectory(${sigar_SOURCE_DIR} ${sigar_BINARY_DIR})
endif ()
