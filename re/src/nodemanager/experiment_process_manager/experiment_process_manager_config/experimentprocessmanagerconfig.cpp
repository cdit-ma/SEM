//
// Created by mitchell on 29/10/20.
//

#include "experimentprocessmanagerconfig.h"
#include <boost/program_options.hpp>
#include <iostream>
namespace sem::experiment_process_manager {

auto ExperimentProcessManagerConfig::ParseArguments(int argc, char** argv)
    -> ExperimentProcessManagerConfig
{
    namespace po = boost::program_options;
    po::options_description command_line_options("Experiment Process Manager options");
    std::string experiment_uuid_string{};
    std::string node_manager_uuid_string{};
    std::string container_uuid_string{};
    std::string control_ip_address_string{};
    std::string data_ip_address_string{};
    std::string process_registration_endpoint_string{};
    std::string experiment_manager_registration_endpoint_string{};
    std::string lib_root_dir_string{};
    std::string re_bin_dir{};

    command_line_options.add_options()("experiment_uuid,e",
                                       po::value<std::string>(&experiment_uuid_string)->required(),
                                       "Experiment uuid");

    command_line_options.add_options()(
        "node_manager_uuid,n", po::value<std::string>(&node_manager_uuid_string)->required(),
        "node manager uuid");

    command_line_options.add_options()("container_uuid,c",
                                       po::value<std::string>(&container_uuid_string)->required(),
                                       "container uuid");

    command_line_options.add_options()("control_ip_address,i",
                                       po::value<std::string>(&control_ip_address_string)->required(),
                                       "control ip address");

    command_line_options.add_options()("data_ip_address,i",
                                       po::value<std::string>(&data_ip_address_string)->required(),
                                       "data ip address");

    command_line_options.add_options()(
        "process_registration_endpoint,p",
        po::value<std::string>(&process_registration_endpoint_string)->required(),
        "endpoint to register process with");

    command_line_options.add_options()(
        "experiment_manager_registration_endpoint,m",
        po::value<std::string>(&experiment_manager_registration_endpoint_string)->required(),
        "endpoint of experiment manager to register with");

    command_line_options.add_options()("lib_root_dir,l",
                                       po::value<std::string>(&lib_root_dir_string)->required(),
                                       "component library root directory path");

    command_line_options.add_options()("re_bin_dir,b",
                                       po::value<std::string>(&re_bin_dir)->required(),
                                       "re binary directory path");

    command_line_options.add_options()("help,h", "help");

    po::variables_map parsed_options;
    po::store(po::parse_command_line(argc, argv, command_line_options), parsed_options);
    po::notify(parsed_options);

    try {
        ExperimentProcessManagerConfig config{
            types::Uuid{},
            types::Uuid{experiment_uuid_string},
            types::Uuid{node_manager_uuid_string},
            types::Uuid{container_uuid_string},
            types::Ipv4(control_ip_address_string),
            types::Ipv4(data_ip_address_string),
            types::SocketAddress(process_registration_endpoint_string),
            types::SocketAddress(experiment_manager_registration_endpoint_string),
            lib_root_dir_string,
            re_bin_dir};
        return config;
    } catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        std::cout << command_line_options << std::endl;
        throw std::invalid_argument("Failed to parse EPM options.");
    }
}
} // namespace sem::experiment_process_manager