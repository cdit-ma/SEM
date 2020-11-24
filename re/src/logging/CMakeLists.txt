find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(std_out_listener)
target_compile_features(std_out_listener PRIVATE cxx_std_17)
target_compile_definitions(std_out_listener PUBLIC BOOST_ALL_DYN_LINK)

target_link_libraries(std_out_listener
        PUBLIC
        sem_types
        sem_grpc_util
#        epm_output_service
#        epm_output_control_service
        Boost::program_options
        )

target_sources(std_out_listener
        PUBLIC
        stdoutlistener.cpp
#        epmoutputimpl.cpp
#        epmoutputimpl.h
        )
