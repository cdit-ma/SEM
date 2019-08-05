SET(QPID_INCLUDE_DIRS
    $ENV{QPID_ROOT}/include
)

FIND_LIBRARY(QPID_MESSAGING
    qpidmessaging
    PATHS
    $ENV{QPID_ROOT}/lib/
)

FIND_LIBRARY(QPID_BROKER
    qpidbroker
    PATHS
    $ENV{QPID_ROOT}/lib/
)

FIND_LIBRARY(QPID_COMMON
    qpidcommon
    PATHS
    $ENV{QPID_ROOT}/lib/
)

FIND_LIBRARY(QPID_TYPES
    qpidtypes
    PATHS
    $ENV{QPID_ROOT}/lib/
)

SET(QPID_LIBRARIES ${QPID_MESSAGING} ${QPID_BROKER} ${QPID_COMMON} ${QPID_TYPES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QPID
    QPID_LIBRARIES 
    QPID_MESSAGING
    QPID_BROKER
    QPID_COMMON
    QPID_TYPES
    QPID_INCLUDE_DIRS
)

mark_as_advanced(
    QPID_LIBRARIES 
    QPID_MESSAGING
    QPID_BROKER
    QPID_COMMON
    QPID_TYPES
)
