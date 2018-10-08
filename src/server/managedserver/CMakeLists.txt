set(PROJ_NAME logan_managedserver)
project (${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

#Link our Logger against all the Cpps
set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/managedserver.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/managedserver.h
    ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

# Compile Definitions
target_compile_definitions(${PROJ_NAME} PUBLIC BOOST_ALL_DYN_LINK)

# Include Directories
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${LOGAN_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Link Libraries 
target_link_libraries(${PROJ_NAME} PRIVATE logan_server_lib)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_environmentrequester)
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARY})