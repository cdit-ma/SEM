# Finds an RTI DDS implementation.
#
# The following directories are used to locate RTI:
# RTI_DDS_ROOT (CMake variable)
# NDDSHOME (Environment variable)
# To find the correct binaries, we also need to know the target of the host binaries:
# RTI_DDS_HOST (CMake variable)
# RTI_DDS_HOST (Environment variable)
#
# ie:
# SET(RTI_DDS_ROOT "/opt/RTI/rti_connext_dds-5.3.0")
# SET(RTI_DDS_HOST "x64Linux2.6gcc4.1.1")

function(RTI_DDS_GENERATE_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: RTI_GENERATE_CPP() called without any idl files")
        return()
    endif()

    #Copy the values from variables defined by SRCS/HDRS into SOURCES/HEADERS respectively
    set(SOURCES ${${SRCS}})
    set(HEADERS ${${HDRS}})

    foreach(FILE ${ARGN})
        # Copy IDL into binary directory
        configure_file(${FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
        get_filename_component(ABS_FILE ${FILE} ABSOLUTE)
        get_filename_component(FILE_WE ${FILE} NAME_WE)

        set(CURRENT_SRCS 
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.cxx"
            # RTI 5.3.0
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}Impl.cxx"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}ImplPlugin.cxx"
            # RTI 5.2.3
            #"${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}Plugin.cxx"
            )

        set(CURRENT_HDRS
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.hpp"
            # RTI 5.3.0
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}Impl.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}ImplPlugin.h"
            # RTI 5.2.3
            #"${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}Plugin.hpp"
        )

        # Append the current srcs/headers into the big list
        list(APPEND SOURCES "${CURRENT_SRCS}")
        list(APPEND HEADERS "${CURRENT_HDRS}")

        if(DEFINED RTI_DDS_IMPORT_DIRS)
            foreach(DIR ${RTI_DDS_IMPORT_DIRS})
                get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
                list(FIND rti_include_path ${ABS_PATH} _contains_already)
                if(${_contains_already} EQUAL -1)
                    list(APPEND rti_include_path -I ${ABS_PATH})
                endif()
            endforeach()
        endif()

        
        add_custom_command(
            OUTPUT  ${CURRENT_SRCS}
                    ${CURRENT_HDRS}
            COMMAND ${RTI_DDS_GEN_EXECUTABLE}
            ARGS -language C++11 -unboundedSupport -update typefiles ${rti_include_path} -d ${CMAKE_CURRENT_BINARY_DIR} ${ABS_FILE}
            DEPENDS ${ABS_FILE} ${RTI_DDS_GEN_EXECUTABLE} 
            COMMENT "Running C++ rtiddsgen on ${FILE}"
            VERBATIM)

        #Set the File properties for the generated files
        set_source_files_properties(${CURRENT_SRCS} ${CURRENT_HDRS} PROPERTIES GENERATED TRUE)
    endforeach()
    
    #Append the source/header files generated into the vars labelled as the parameters passed into the function
    set(${SRCS} ${SOURCES} PARENT_SCOPE)
    set(${HDRS} ${HEADERS} PARENT_SCOPE)
endfunction()

if(NOT RTI_DDS_ROOT)
    # If RTI_DDS_ROOT has not been set in CMAKE, use the environment variable NDDSHOME
    SET(RTI_DDS_ROOT $ENV{NDDSHOME})
endif()

if(NOT RTI_DDS_ROOT)
    # If RTI_DDS_ROOT has not been set in CMAKE, use the environment variable NDDSHOME
    SET(RTI_DDS_ROOT $ENV{NDDSHOME})
endif()



if(NOT RTI_DDS_ROOT)
    message(WARNING "Either RTI_DDS_ROOT or NDDSHOME are not set, have you run the RTI configure script?")
    return()
endif()

if(NOT RTI_DDS_HOST)
    # If RTI_DDS_HOST has not been set in CMAKE, use the environment variable RTI_DDS_HOST
    SET(RTI_DDS_HOST $ENV{RTI_DDS_HOST})
endif()

if(NOT RTI_DDS_HOST)
    message(STATUS "Error: Variable RTI_DDS_HOST not set.")
    return()
endif()

find_path(RTI_DDS_INCLUDE_DIR NAMES ndds/ndds_cpp.h HINTS ${RTI_DDS_ROOT}/include)

find_library(RTI_DDS_C_LIBRARY NAMES nddsc HINTS ${RTI_DDS_ROOT}/lib PATH_SUFFIXES ${RTI_DDS_HOST})
find_library(RTI_DDS_CPP_LIBRARY NAMES nddscpp HINTS ${RTI_DDS_ROOT}/lib PATH_SUFFIXES ${RTI_DDS_HOST})
find_library(RTI_DDS_CPP2_LIBRARY NAMES nddscpp2 HINTS ${RTI_DDS_ROOT}/lib PATH_SUFFIXES ${RTI_DDS_HOST})
find_library(RTI_DDS_CORE_LIBRARY NAMES nddscore HINTS ${RTI_DDS_ROOT}/lib PATH_SUFFIXES ${RTI_DDS_HOST})

find_program(RTI_DDS_GEN_EXECUTABLE
    NAMES rtiddsgen
    DOC "The RTI DDS IDL compiler"
    HINTS
    $ENV{NDDSHOME}/bin)

if(WIN32)
    set(RTI_DDS_GEN_EXECUTABLE ${RTI_DDS_ROOT}/bin/rtiddsgen.bat)
    set(RTI_DDS_EXTRA_LIBRARIES netapi32 advapi32 user32 ws2_32)
    set(RTI_DDS_DEFINITIONS "-DRTI_WIN32 -DNDDS_DLL_VARIABLE /MD")
elseif(UNIX AND NOT APPLE)
    set(RTI_DDS_DEFINITIONS "-DRTI_UNIX -DRTI_64BIT")
    set(RTI_DDS_EXTRA_LIBRARIES dl nsl m pthread rt)
elseif(APPLE)
    set(RTI_DDS_DEFINITIONS "-DRTI_UNIX -DRTI_64BIT")
endif(WIN32)

set(RTI_DDS_INCLUDE_DIRS ${RTI_DDS_INCLUDE_DIR} ${RTI_DDS_INCLUDE_DIR}/ndds ${RTI_DDS_INCLUDE_DIR}/ndds/hpp)
set(RTI_DDS_LIBRARIES ${RTI_DDS_C_LIBRARY} ${RTI_DDS_CPP_LIBRARY} ${RTI_DDS_CPP2_LIBRARY} ${RTI_DDS_CORE_LIBRARY} ${RTI_DDS_EXTRA_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RTI_DDS REQUIRED_VARS RTI_DDS_ROOT RTI_DDS_LIBRARIES RTI_DDS_INCLUDE_DIRS RTI_DDS_GEN_EXECUTABLE)

