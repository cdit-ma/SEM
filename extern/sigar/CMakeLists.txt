
# SIGAR
if (NO_SIGAR)
    return()
endif ()
message(STATUS "Fetching SIGAR")
FetchContent_Declare(sigar
        GIT_REPOSITORY https://github.com/cdit-ma/sigar
        # TODO: Point at a release tag, this is currently pointing at cdit-ma's fork of sigar master.
        GIT_TAG 4e26f8ff2d4b1addbf629113df83b78ccdfa3c26
        GIT_SHALLOW TRUE
        )
if (NOT sigar_POPULATE)
    FetchContent_Populate(sigar)
    add_subdirectory(${sigar_SOURCE_DIR} ${sigar_BINARY_DIR})
endif ()
