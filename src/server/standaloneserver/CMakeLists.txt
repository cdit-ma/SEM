cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJ_NAME logan_server)
project (${PROJ_NAME})

#Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)


find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)
find_package(SIGAR REQUIRED)

#Need boost > 1.30.0 system and filesystem for crossplatform temp file creation.
find_package(Boost 1.30.0 COMPONENTS program_options filesystem system REQUIRED)

#Link our Logger against all the Cpps
set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

# Link against boost dynamically
target_compile_definitions(${PROJ_NAME} PRIVATE -DBOOST_ALL_DYN_LINK)

if (MSVC)
    # Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
    # Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
else()
    # Unix needs to include pthread
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()


#include the current binary directory to re_node_manager as it requires the .h file from the pb generated files
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${LOGAN_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_BINARY_DIR})


target_link_libraries(${PROJ_NAME} logan_server_lib)

target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})