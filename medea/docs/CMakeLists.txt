find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
    message(STATUS "Cannot find doxygen, not building documentation")
    return()
endif()

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/)
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Populate list of public headers. Used as dependency of documentation build command, ensures that docs are rebuilt
#  on public interface change.
file(GLOB_RECURSE PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h*)

# Doxyfile configuration, replaces instances of "@CMAKE_VAR_NAME@" with "value_of_cmake_var"
set(DOXYGEN_CONFIG_PROJECT_NAME "CDIT-MA ${PROJECT_NAME}")
set(DOXYGEN_CONFIG_PROJECT_DESCRIPTION "Distributed system graphical modelling tool.")
set(DOXYGEN_CONFIG_PROJECT_VERSION ${PROJECT_VERSION})
set(DOXYGEN_CONFIG_MAIN_PAGE ${PROJECT_SOURCE_DIR}/README.md)
set(DOXYGEN_CONFIG_INPUT_DIR "${PROJECT_SOURCE_DIR}/src/ \
                              ${PROJECT_SOURCE_DIR}/README.md \
                              ${PROJECT_SOURCE_DIR}/VERSION.md")
set(DOXYGEN_CONFIG_OUTPUT_DIR ${DOXYGEN_OUTPUT_DIR})

set(DOXYGEN_CONFIG_EXCLUDES "")

# To extend this list take note of problem highlighted by
#  https://stackoverflow.com/questions/5507055/excluding-directories-for-doxygen
# Ideally there will be less need to exclude entire directories from documentation in the future
set(DOXYGEN_CONFIG_EXCLUDE_PATTERNS "")

# Replace instances of DOXYGEN_CONFIG_ prefixed configure args
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
# End doxygen file configuration

set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)

add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
        DEPENDS ${PUBLIC_HEADERS}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
        COMMENT "Generating docs"
        VERBATIM)

add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
