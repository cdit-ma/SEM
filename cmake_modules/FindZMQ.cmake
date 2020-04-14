# - Try to find ZMQ headers and libraries
# - THANKS CUBIT FOR THIS FIND MODULE
#
# Usage of this module as follows:
#
#     find_package(ZMQ)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  ZMQ_ROOT_DIR  Set this variable to the root installation of
#                            ZMQ if the module has problems finding
#                            the proper installation path.
#
# Variables defined by this module:
#
#  ZMQ_FOUND              System has ZMQ libs/headers
#  ZMQ_LIBRARIES          The ZMQ libraries
#  ZMQ_INCLUDE_DIRS       The location of ZMQ headers

# This method for finding ZMQ is not a cross platform solution, the officially provided option is the better choice

find_package(ZeroMQ)

if (ZeroMQ_FOUND)
    message("ZeroMQ found, using the configurations to override those previously populated by FindZMQ.cmake")

    set(ZMQ_INCLUDE_DIRS ${ZeroMQ_INCLUDE_DIRS})

    set(ZMQ_LIBRARIES
            ${ZeroMQ_LIBRARY}
            ${ZeroMQ_LIBRARY_STATIC})

else()
    message(WARNING "Use of find_package(ZMQ) is deprecated, recommend replacing with find_package(ZeroMQ)")

    find_path(ZMQ_ROOT_DIR
            NAMES include/zmq.h
            )

    find_library(ZMQ_LIBRARIES
            NAMES zmq libzmq
            HINTS ${ZMQ_ROOT_DIR}/lib
            )

    find_path(ZMQ_INCLUDE_DIRS
            NAMES zmq.h
            HINTS ${ZMQ_ROOT_DIR}/include
            )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ZMQ DEFAULT_MSG
            ZMQ_LIBRARIES
            ZMQ_INCLUDE_DIRS
            )
endif()



mark_as_advanced(
#    ZMQ_ROOT_DIR
    ZMQ_LIBRARIES
    ZMQ_INCLUDE_DIRS
)

