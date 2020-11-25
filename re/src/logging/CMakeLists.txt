find_package(Boost 1.67.0 COMPONENTS program_options REQUIRED)

add_executable(std_out_listener)
target_compile_features(std_out_listener PRIVATE cxx_std_17)

target_link_libraries(std_out_listener
        PUBLIC
        sem_types
        sem_grpc_util
#        epm_output_service
#        epm_output_control_service
        Boost::program_options
        Boost::dynamic_linking
        )

target_sources(std_out_listener
        PUBLIC
        stdoutlistener.cpp
#        epmoutputimpl.cpp
#        epmoutputimpl.h
        )
