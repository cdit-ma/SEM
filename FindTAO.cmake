
function(TAO_GENERATE_CPP SRCS HDRS)
    #Copy the values from variables defined by SRCS/HDRS into SOURCES/HEADERS respectively
    set(SOURCES ${${SRCS}})
    set(HEADERS ${${HDRS}})

    foreach(FILE ${ARGN})
        # Copy IDL into binary directory
        configure_file(${FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
        get_filename_component(ABS_FILE ${FILE} ABSOLUTE)
        get_filename_component(FIL_WE ${FILE} NAME_WE)

        set(CURRENT_SRCS 
            "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}C.cpp"
            "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}S.cpp"
            )
        set(CURRENT_HDRS
            "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}C.h"
            "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}S.h"
        )

        # Append the current srcs/headers into the big list
        list(APPEND SOURCES "${CURRENT_SRCS}")
        list(APPEND HEADERS "${CURRENT_HDRS}")

        if(DEFINED TAO_IMPORT_DIRS)
            foreach(DIR ${TAO_IMPORT_DIRS})
                get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
                list(FIND tao_include_path ${ABS_PATH} _contains_already)
                if(${_contains_already} EQUAL -1)
                    list(APPEND tao_include_path -I ${ABS_PATH})
                endif()
            endforeach()
        endif()
        
        add_custom_command(
            OUTPUT  ${CURRENT_SRCS}
                    ${CURRENT_HDRS}
            COMMAND ${TAO_GEN_EXECUTABLE}
            ARGS -Cw ${tao_include_path} -o ${CMAKE_CURRENT_BINARY_DIR} ${ABS_FILE}
            DEPENDS ${ABS_FILE} ${TAO_GEN_EXECUTABLE} 
            COMMENT "Running tao_idl on ${FILE}"
            VERBATIM)

        #Set the File properties for the generated files
        set_source_files_properties(${CURRENT_SRCS} ${CURRENT_HDRS} PROPERTIES GENERATED TRUE)
    endforeach()
    
    #Append the source/header files generated into the vars labelled as the parameters passed into the function
    set(${SRCS} ${SOURCES} PARENT_SCOPE)
    set(${HDRS} ${HEADERS} PARENT_SCOPE)
endfunction()

if(NOT ACE_ROOT)
    SET(ACE_ROOT $ENV{ACE_ROOT})
endif()

if(NOT ACE_ROOT)
    message(STATUS "Error: ACE_ROOT not set as an Environment Variable")
    return()
endif()

if(NOT TAO_ROOT)
    SET(TAO_ROOT $ENV{TAO_ROOT})
endif()

if(NOT TAO_ROOT)
    message(STATUS "Error: TAO_ROOT not set as an Environment Variable")
    return()
endif()


find_library(TAO_LIBRARY NAMES TAO HINTS ${ACE_ROOT}/lib)
find_library(ACE_LIBRARY NAMES ACE HINTS ${ACE_ROOT}/lib)
find_library(TAO_PS_LIBRARY NAMES TAO_PortableServer HINTS ${ACE_ROOT}/lib)
find_library(TAO_ATC_LIBRARY NAMES TAO_AnyTypeCode HINTS ${ACE_ROOT}/lib)
find_library(TAO_IOR_LIBRARY NAMES TAO_IORTable HINTS ${ACE_ROOT}/lib)
find_library(TAO_MESSAGING_LIBRARY NAMES TAO_Messaging HINTS ${ACE_ROOT}/lib)
find_library(TAO_VALUE_TYPE NAMES TAO_Valuetype HINTS ${ACE_ROOT}/lib)

find_program(TAO_GEN_EXECUTABLE
        NAMES tao_idl
        DOC "The TAO idl compiler"
        HINTS ${ACE_ROOT}/bin)

set(TAO_INCLUDE_DIRS ${ACE_ROOT} ${TAO_ROOT})
set(TAO_LIBRARIES ${TAO_LIBRARY} ${ACE_LIBRARY} ${TAO_PS_LIBRARY} ${TAO_ATC_LIBRARY} ${TAO_IOR_LIBRARY} ${TAO_MESSAGING_LIBRARY} ${TAO_VALUE_TYPE})
 
include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(TAO
    REQUIRED_VARS TAO_GEN_EXECUTABLE TAO_INCLUDE_DIRS TAO_LIBRARIES)
