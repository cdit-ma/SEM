# - Try to find KDIS headers and libraries
#
# Usage of this module as follows:
#
#     find_package(KDIS)
#
# Variables defined by this module:
#
#  KDIS_FOUND              System has KDIS libs/headers
#  KDIS_LIBRARIES          The KDIS libraries
#  KDIS_INCLUDE_DIRS       The location of KDIS headers

find_path(KDIS_ROOT_DIR
        NAMES include/KDIS/KDefines.h
        )

find_library(KDIS_LIBRARIES
        NAMES kdis
        )

find_path(KDIS_INCLUDE_DIRS
        NAMES KDIS/KDefines.h
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KDIS DEFAULT_MSG
        KDIS_LIBRARIES
        KDIS_INCLUDE_DIRS
        )

mark_as_advanced(
        KDIS_ROOT_DIR
        KDIS_LIBRARIES
        KDIS_INCLUDE_DIRS
)
