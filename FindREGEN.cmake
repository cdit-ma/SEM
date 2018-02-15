
find_package(Java COMPONENTS Runtime)
include(UseJava)
include(FindPackageHandleStandardArgs)

SET(DATATYPES_XSL "g2datatypes.xsl")
SET(COMPONENTS_XSL "g2components.xsl")

function(REGEN_GENERATE_CPP REGEN_GENERATED_FOLDERS)
    #Copy the values from SRCS and HDRS into src,hdrs
    set(${REGEN_GENERATED_FOLDERS} ${${REGEN_GENERATED_FOLDERS}})
    set(WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR})

    foreach(FILE ${ARGN})
        # Copy file into Binary dir
        configure_file(${FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
        get_filename_component(ABS_FIL ${FILE} ABSOLUTE)
        get_filename_component(GRAPHML_PATH ${FILE} NAME)
        
        set(GRAPHML_FILE ${CMAKE_CURRENT_BINARY_DIR}/${GRAPHML_PATH})
        
        message("XSL Generating C++ Datatypes: ${GRAPHML_PATH} in ${WORKING_DIR}")
        execute_process(
            COMMAND ${Java_JAVA_EXECUTABLE} -jar ${REGEN_PATH}/saxon.jar -xsl:${REGEN_PATH}/${DATATYPES_XSL} -s:${GRAPHML_FILE}
            WORKING_DIRECTORY "${WORKING_DIR}/"
            OUTPUT_QUIET
            ERROR_QUIET
        )

        message("XSL Generating C++ Components: ${GRAPHML_PATH}")
        execute_process(
            COMMAND ${Java_JAVA_EXECUTABLE} -jar ${REGEN_PATH}/saxon.jar -xsl:${REGEN_PATH}/${COMPONENTS_XSL} -s:${GRAPHML_FILE}
            WORKING_DIRECTORY "${WORKING_DIR}/"
            OUTPUT_QUIET
            ERROR_QUIET
        )
    endforeach()
    list(APPEND ${REGEN_GENERATED_FOLDERS} "${WORKING_DIR}/datatypes")
    list(APPEND ${REGEN_GENERATED_FOLDERS} "${WORKING_DIR}/components")

    set(${REGEN_GENERATED_FOLDERS} ${${REGEN_GENERATED_FOLDERS}} PARENT_SCOPE)
endfunction()

if(NOT REGEN_PATH)
    # Set REGEN_PATH from env if not set
    SET(REGEN_PATH $ENV{REGEN_PATH})
endif(NOT REGEN_PATH)


if(NOT REGEN_PATH)
    message(STATUS "Error: REGEN_PATH not set as an Environment Variable")
    return()
endif()

# Find the REGEN_PATH, by looking for the transforms
find_path(REGEN_PATH ${DATATYPES_XSL} ${COMPONENTS_XSL} HINTS ${REGEN_PATH})

# Find the SAXON jar file from the REGEN_PATH
find_jar(SAXON_JAR name saxon PATHS ${REGEN_PATH})

# Set the package as found
find_package_handle_standard_args(REGEN DEFAULT_MSG
    Java_FOUND
    REGEN_PATH
    SAXON_JAR
)
