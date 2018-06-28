find_package(Java COMPONENTS Runtime)
include(UseJava)
include(FindPackageHandleStandardArgs)

SET(GENERATE_XSL "generate_project.xsl")

function(REGEN_GENERATE_CPP)
    set(oneValueArgs GRAPHML PROJECT_PREFIX GENERATED_DIRS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT ARGS_GENERATED_DIRS)
        message(SEND_ERROR "Error: REGEN_GENERATE_CPP() called without a GENERATED_DIRS")
        return()
    endif()

    set(WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR})
    
    # Copy file into Binary dir
    get_filename_component(ABS_FIL ${ARGS_GRAPHML} ABSOLUTE)
    get_filename_component(GRAPHML_PATH ${ARGS_GRAPHML} NAME)
    
    execute_process(
        COMMAND ${Java_JAVA_EXECUTABLE} -jar ${REGEN_PATH}/saxon.jar -xsl:${REGEN_PATH}/${GENERATE_XSL} -s:${ABS_FIL} library_prefix=${ARGS_PROJECT_PREFIX}
        WORKING_DIRECTORY "${WORKING_DIR}/"
        OUTPUT_QUIET
        ERROR_VARIABLE GENERATE_ERROR
        RESULT_VARIABLE GENERATE_SUCCESS
        ERROR_STRIP_TRAILING_WHITESPACE
    )

    if(${GENERATE_SUCCESS} STREQUAL "0")
        message(STATUS "RE_GEN: Generated C++ for model: ${GRAPHML_PATH} in: ${WORKING_DIR}")
    else()
        message(SEND_ERROR "RE_GEN: Failed generating C++ for model: ${GRAPHML_PATH} in: ${WORKING_DIR}")
        message("--->  ${GENERATE_XSL} Error Output [START] <---")
        message(${GENERATE_ERROR})
        message("--->  ${GENERATE_XSL} Error Output  [END]  <---")
        return()
    endif()
    list(APPEND ${ARGS_GENERATED_DIRS} "${WORKING_DIR}/classes")
    list(APPEND ${ARGS_GENERATED_DIRS} "${WORKING_DIR}/components")
    list(APPEND ${ARGS_GENERATED_DIRS} "${WORKING_DIR}/datatypes")
    list(APPEND ${ARGS_GENERATED_DIRS} "${WORKING_DIR}/ports")

    set(${ARGS_GENERATED_DIRS} ${${ARGS_GENERATED_DIRS}} PARENT_SCOPE)
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
find_path(REGEN_PATH ${GENERATE_XSL} HINTS ${REGEN_PATH})

# Find the SAXON jar file from the REGEN_PATH
find_jar(SAXON_JAR name saxon PATHS ${REGEN_PATH})

# Set the package as found
find_package_handle_standard_args(REGEN DEFAULT_MSG
    Java_FOUND
    REGEN_PATH
    SAXON_JAR
)
