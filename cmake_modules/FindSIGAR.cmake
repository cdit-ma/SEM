find_path(SIGAR_ROOT_DIR
    NAMES include/sigar.h
)

find_library(SIGAR_LIBRARIES
    NAMES sigar libsigar
    HINTS ${SIGAR_ROOT_DIR}/build-src
    HINTS ${SIGAR_ROOT_DIR}/lib
)

find_path(SIGAR_INCLUDE_DIRS
    NAMES sigar.h
    HINTS ${SIGAR_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sigar DEFAULT_MSG
    SIGAR_LIBRARIES
    SIGAR_INCLUDE_DIRS
)

mark_as_advanced(
    SIGAR_ROOT_DIR
    SIGAR_LIBRARIES
    SIGAR_INCLUDE_DIRS
)
