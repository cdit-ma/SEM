# Finds an OpenSplice DDS implementation.
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

function(OSPL_DDS_GENERATE_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: OSPL_GENERATE_CPP() called without any idl files")
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
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.cpp"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}Dcps.cpp"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}SplDcps.cpp"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}Dcps_impl.cpp"
            )

        set(CURRENT_HDRS
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}Dcps.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}SplDcps.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}Dcps_impl.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_DCPS.hpp"
        )

        # Append the current srcs/headers into the big list
        list(APPEND SOURCES "${CURRENT_SRCS}")
        list(APPEND HEADERS "${CURRENT_HDRS}")

        if(DEFINED OSPL_DDS_IMPORT_DIRS)
            foreach(DIR ${OSPL_DDS_IMPORT_DIRS})
                get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
                list(FIND ospl_include_path ${ABS_PATH} _contains_already)
                if(${_contains_already} EQUAL -1)
                    list(APPEND ospl_include_path -I ${ABS_PATH})
                endif()
            endforeach()
        endif()
        
        add_custom_command(
            OUTPUT  ${CURRENT_SRCS}
                    ${CURRENT_HDRS}
            COMMAND ${OSPL_DDS_GEN_EXECUTABLE}
            ARGS -l isocpp -S ${ABS_FILE} -d ${CMAKE_CURRENT_BINARY_DIR}
            ${ospl_include_path}
            DEPENDS ${ABS_FILE} ${OSPL_DDS_GEN_EXECUTABLE} 
            COMMENT "Running C++ ospl dds on ${FILE}"
            VERBATIM)

        #Set the File properties for the generated files
        set_source_files_properties(${CURRENT_SRCS} ${CURRENT_HDRS} PROPERTIES GENERATED TRUE)
    endforeach()
    
    #Append the source/header files generated into the vars labelled as the parameters passed into the function
    set(${SRCS} ${SOURCES} PARENT_SCOPE)
    set(${HDRS} ${HEADERS} PARENT_SCOPE)
endfunction()

if(NOT OSPL_DDS_ROOT)
    # If OSPL_DDS_ROOT has not been set in CMAKE, use the environment variable OSPL_HOME
    SET(OSPL_DDS_ROOT $ENV{OSPL_HOME})
endif()

if(NOT OSPL_DDS_ROOT)
    message(STATUS "Error: Either OSPL_DDS_ROOT or OSPL_HOME are not set, have you run the OpenSplice configure script?")
    return()
endif()


find_path(OSPL_DDS_INCLUDE_DIR NAMES dcps/C++/isocpp/dds/dds.hpp HINTS ${OSPL_DDS_ROOT}/include)
find_library(OSPL_DDS_CPP_LIBRARY NAMES dcpsisocpp HINTS ${OSPL_DDS_ROOT}/lib)
find_library(OSPL_DDS_CORE_LIBRARY NAMES ddskernel HINTS ${OSPL_DDS_ROOT}/lib)

find_program(OSPL_DDS_GEN_EXECUTABLE
    NAMES idlpp
    DOC "The OpenSplice DDS IDL compiler"
    HINTS
    ${OSPL_DDS_ROOT}/bin)

set(OSPL_DDS_INCLUDE_DIRS ${OSPL_DDS_INCLUDE_DIR} ${OSPL_DDS_INCLUDE_DIR}/sys ${OSPL_DDS_INCLUDE_DIR}/dcps/C++/isocpp ${OSPL_DDS_INCLUDE_DIR}/dcps/C++/SACPP)
set(OSPL_DDS_LIBRARIES ${OSPL_DDS_CPP_LIBRARY} ${OSPL_DDS_CORE_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OSPL_DDS 
    REQUIRED_VARS OSPL_DDS_ROOT OSPL_DDS_LIBRARIES OSPL_DDS_INCLUDE_DIRS OSPL_DDS_GEN_EXECUTABLE)