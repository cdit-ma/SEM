
# TODO: Check for new kernel version that doesn't support SIGAR
# SIGAR
message(STATUS "Fetching SIGAR")
FetchContent_Declare(sigar)
if (NOT sigar_POPULATE)
    FetchContent_Populate(sigar
            GIT_REPOSITORY https://github.com/cdit-ma/sigar
            GIT_TAG master
            GIT_SHALLOW TRUE
            )
    add_subdirectory(${sigar_SOURCE_DIR} ${sigar_BINARY_DIR})
endif ()
