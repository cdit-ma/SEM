set(PROJ_NAME logan_server_lib)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_library(${PROJ_NAME} STATIC "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

# Compile with bigobj on windows (for sqlite amalgamation file)
if(MSVC)
    target_compile_options(${PROJ_NAME} "/bigobj")
endif(MSVC)

# Disable model logging if flag set during cmake generation
if(NOT DISABLE_MODEL_LOGGING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/protohandlers/modelevent")
    target_link_libraries(${PROJ_NAME} PUBLIC logan_protohandler_modelevent)
else()
    target_compile_definitions(${PROJ_NAME} PUBLIC DISABLE_MODEL_HANDLER)
endif()

# Disable hardware logging if flag set during cmake generation
if(NOT DISABLE_HARDWARE_LOGGING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/protohandlers/systemevent")
    target_link_libraries(${PROJ_NAME} PUBLIC logan_protohandler_systemevent)
else()
    target_compile_definitions(${PROJ_NAME} PUBLIC DISABLE_HARDWARE_HANDLER)
endif()

target_sources(${PROJ_NAME} PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c

        # Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/protohandler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.h
        ${CMAKE_CURRENT_SOURCE_DIR}/table.h
        ${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.h
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h
    )

# Include source dirs s.t. sql headers are available when we build tests
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)

if(UNIX)
    target_link_libraries(${PROJ_NAME} PUBLIC dl)
endif()
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_control)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/managedserver")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/standaloneserver")
