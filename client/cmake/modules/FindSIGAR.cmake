# - Try to find ZeroMQ headers and libraries
# - THANKS CUBIT FOR THIS FIND MODULE
#
# Usage of this module as follows:
#
#     find_package(ZeroMQ)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  ZeroMQ_ROOT_DIR  Set this variable to the root installation of
#                            ZeroMQ if the module has problems finding
#                            the proper installation path.
#
# Variables defined by this module:
#
#  ZEROMQ_FOUND              System has ZeroMQ libs/headers
#  ZeroMQ_LIBRARIES          The ZeroMQ libraries
#  ZeroMQ_INCLUDE_DIRS       The location of ZeroMQ headers

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
