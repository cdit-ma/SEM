##############################################################################
# Try to find OpenSplice
# Once done this will define:
#
#  OpenSplice_FOUND - system has OpenSplice.
#  OPENSPLICE_INCLUDE_DIRS - the OpenSplice include directory.
#  OPENSPLICE_LIBRARIES - Link these to use OpenSplice.
#  OpenSplice_IDLGEN_BINARY - Binary for the IDL compiler.
#
# You need the environment variable $OSPL_HOME to be set to your OpenSplice
# installation directory.
# This script also includes the MacroOpenSplice.cmake script, which is useful
# for generating code from your idl.
#
##############################################################################
# Courtesy of Ivan Galvez Junquera <ivgalvez@gmail.com>
##############################################################################


function(OSPL_GENERATE_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: OSPL_GENERATE_CPP() called without any idl files")
        return()
    endif()

    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        # Copy IDL into binary directory
        configure_file(${FIL} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)

        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.cpp")
        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}_DCPS.hpp")
        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}Dcps.cpp")
        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}Dcps_impl.cpp")
        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}SplDcps.cpp")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.h")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}Dcps.h")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}Dcps_impl.h")
        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}SplDcps.h")

        add_custom_command(
        OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}_DCPS.hpp"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}Dcps.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}Dcps_impl.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}SplDcps.cpp"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.h"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}Dcps.h"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}Dcps_impl.h"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}SplDcps.h"               
        COMMAND  ${OSPL_GEN_EXECUTABLE}
        ARGS -l isocpp -S ${ABS_FIL} -d ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${ABS_FIL} ${OSPL_GEN_EXECUTABLE}
        COMMENT "Running C++ idlpp on ${FIL}"
        VERBATIM )
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)

endfunction()








SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(SPLICE_TARGET x86_64.linux)
FIND_PATH(OpenSplice_INCLUDE_DIR
NAMES
ccpp_dds_dcps.h
#dds_dcps.idl
PATHS
$ENV{OSPL_HOME}/include/dcps/C++/isocpp
#$ENV{OSPL_HOME}/etc/idl/
)

SET(OPENSPLICE_INCLUDE_DIRS
$ENV{OSPL_HOME}/include/sys
$ENV{OSPL_HOME}/include/dcps/C++/isocpp
$ENV{OSPL_HOME}/include/dcps/C++/SACPP
)

# Find libraries
FIND_LIBRARY(DCPSISOCPP
NAMES
dcpsisocpp
PATHS
$ENV{OSPL_HOME}/lib/${SPLICE_TARGET}
)

FIND_LIBRARY(DDSKERNEL
NAMES
ddskernel
PATHS
$ENV{OSPL_HOME}/lib/${SPLICE_TARGET}
)

SET(OPENSPLICE_LIBRARIES
${DCPSISOCPP}
${DDSKERNEL}
dl
pthread
)


find_program(OSPL_GEN_EXECUTABLE
        NAMES idlpp
        DOC "The ospl idl compiler"
        PATHS
        $ENV{OSPL_HOME}/bin)

# Binary for the IDL compiler
# SET (OpenSplice_IDLGEN_BINARY $ENV{OSPL_HOME}/exec/${SPLICE_TARGET}/idlpp -I $ENV{OSPL_HOME}/etc/idl/)

IF (OPENSPLICE_INCLUDE_DIRS AND OPENSPLICE_LIBRARIES)
SET(OpenSplice_FOUND TRUE)
ENDIF (OPENSPLICE_INCLUDE_DIRS AND OPENSPLICE_LIBRARIES)

IF (OpenSplice_FOUND)
MESSAGE(STATUS "Found OpenSplice DDS libraries: ${OPENSPLICE_LIBRARIES}")
ELSE (OpenSplice_FOUND)
IF (OpenSplice_FIND_REQUIRED)
MESSAGE(FATAL_ERROR "Could not find OpenSplice DDS")
ENDIF (OpenSplice_FIND_REQUIRED)
ENDIF (OpenSplice_FOUND)

#MARK_AS_ADVANCED(OPENSPLICE_INCLUDE_DIRS OPENSPLICE_LIBRARIES OpenSplice_IDLGEN_BINARY)
INCLUDE (MacroOpenSplice)