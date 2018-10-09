set(PROJ_NAME "re_node_manager")
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options date_time chrono REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loganclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loganlogger.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loganclient.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.h
    ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loganlogger.h
    )

# Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

# Compile Definitions
target_compile_definitions(${PROJ_NAME} PRIVATE BOOST_ALL_DYN_LINK)

# Include Directories
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${LOGAN_SOURCE_PATH})

# Link Libraries
target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_protoreplier)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_protowriter)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_environmentrequester)
target_link_libraries(${PROJ_NAME} PRIVATE logan_client_lib)
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_DATE_TIME_LIBRARY})
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_CHRONO_LIBRARY})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
    target_link_libraries(${PROJ_NAME} PRIVATE dl)
endif(UNIX)