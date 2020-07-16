find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(logan_managedserver)
target_compile_features(logan_managedserver PRIVATE cxx_std_11)
target_compile_definitions(logan_managedserver PUBLIC BOOST_ALL_DYN_LINK)

target_sources(logan_managedserver 
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/managedserver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/managedserver.h
        )
target_link_libraries(logan_managedserver
        PRIVATE
        sem_version
        logan_server_lib
        zmq_environmentrequester
        Boost::program_options
        execution_helper
        zmq_environmentrequester
        )
