set(PROJ_NAME "re_node_manager")
set(LIB_NAME "re_node_manager_lib")
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

# Node manager library
add_library(${LIB_NAME} SHARED "")
target_compile_features(${LIB_NAME} PRIVATE cxx_std_11)
target_compile_definitions(${LIB_NAME} PUBLIC BOOST_ALL_DYN_LINK)

target_sources(${LIB_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loganclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/slaveheartbeater.cpp
    
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loganclient.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.h
    ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h
    ${CMAKE_CURRENT_SOURCE_DIR}/slaveheartbeater.h
)
target_include_directories(${LIB_NAME} PUBLIC ${LOGAN_SOURCE_PATH})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${LIB_NAME} PUBLIC re_core)
target_link_libraries(${LIB_NAME} PUBLIC re_loggers)
target_link_libraries(${LIB_NAME} PUBLIC zmq_protoreceiver)
target_link_libraries(${LIB_NAME} PUBLIC zmq_protoreplier)
target_link_libraries(${LIB_NAME} PUBLIC zmq_protowriter)
target_link_libraries(${LIB_NAME} PUBLIC zmq_environmentrequester)
target_link_libraries(${LIB_NAME} PUBLIC logan_client_lib)

# Link against pthread if unix
if(UNIX)
    target_link_libraries(${LIB_NAME} PRIVATE pthread)
    target_link_libraries(${LIB_NAME} PRIVATE dl)
endif(UNIX)

# Node manager executable
add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager.cpp
)
target_link_libraries(${PROJ_NAME} PUBLIC ${LIB_NAME})
target_link_libraries(${PROJ_NAME} PUBLIC re_core_singletons)
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARY})

# Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)
