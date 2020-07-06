find_path(Sigar_ROOT_DIR
    NAMES include/sigar.h
)

find_library(Sigar_LIBRARIES
    NAMES sigar libsigar
    HINTS ${Sigar_ROOT_DIR}/build-src
    HINTS ${Sigar_ROOT_DIR}/lib
)

find_path(Sigar_INCLUDE_DIRS
    NAMES sigar.h
    HINTS ${Sigar_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sigar DEFAULT_MSG
    Sigar_LIBRARIES
    Sigar_INCLUDE_DIRS
)

mark_as_advanced(
    Sigar_ROOT_DIR
    Sigar_LIBRARIES
    Sigar_INCLUDE_DIRS
)
