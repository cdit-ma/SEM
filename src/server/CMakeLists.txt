set(PROJ_NAME logan_server_lib)
set(SQL_LIB sqlite_lib)
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)


SET(SQLITE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h
)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.cpp
    )

SET(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protohandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/table.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.h
    )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

# Construct a shared library
add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})
add_library(${SQL_LIB} STATIC ${SQLITE_SOURCE})

if(NOT DISABLE_MODEL_LOGGING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/modelprotohandler")
    target_link_libraries(${PROJ_NAME} PUBLIC modelprotohandler)
else()
    target_compile_definitions(${PROJ_NAME} PUBLIC DISABLE_MODEL_HANDLER)
endif()

if(NOT DISABLE_HARDWARE_LOGGING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/hardwareprotohandler")
    target_link_libraries(${PROJ_NAME} PUBLIC hardwareprotohandler)
else()
    target_compile_definitions(${PROJ_NAME} PUBLIC DISABLE_HARDWARE_HANDLER)
endif()

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_systemstatus)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_control)
target_link_libraries(${PROJ_NAME} PUBLIC sqlite_lib)

# Add the Subdirectories
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/managedserver")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/standaloneserver")

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
endif(MSVC)