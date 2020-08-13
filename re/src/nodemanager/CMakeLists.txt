find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

# Node manager library
add_library(re_node_manager_lib SHARED)
target_compile_features(re_node_manager_lib PRIVATE cxx_std_17)
target_compile_definitions(re_node_manager_lib PUBLIC BOOST_ALL_DYN_LINK)

target_sources(re_node_manager_lib PRIVATE
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
    ${CMAKE_CURRENT_SOURCE_DIR}/slaveheartbeater.h
)
target_include_directories(re_node_manager_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(re_node_manager_lib
        PUBLIC
        re_loggers
        re_core
        zmq_protoreceiver
        zmq_protoreplier
        zmq_protowriter
        zmq_environmentrequester
        execution_helper
        PRIVATE
        logan_client_lib
        )

# Link against pthread if unix
if(UNIX)
    target_link_libraries(re_node_manager_lib PRIVATE pthread)
    target_link_libraries(re_node_manager_lib PRIVATE dl)
endif(UNIX)

# Node manager executable
add_executable(re_node_manager)
target_compile_features(re_node_manager PRIVATE cxx_std_11)

target_sources(re_node_manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager.cpp
)
target_link_libraries(re_node_manager
        PRIVATE
        re_core_singletons
        re_node_manager_lib
        Boost::program_options
        sem_version
        )
