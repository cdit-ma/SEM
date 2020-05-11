#include "nodemanagerconfig.h"
#include "nodemanager.h"
#include <boost/asio/io_service.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/process/io.hpp>
#include <boost/process/search_path.hpp>
#include <boost/program_options.hpp>
#include <comms/environmentrequester/environmentrequester.h>
#include <fstream>
#include <iostream>
#include <network/protocols/nodemanagerregistration/nodemanagerregistration.pb.h>
#include <network/requester.hpp>
#include <optional>
#include <utility>
auto re::NodeManager::NodeConfig::FromIstream(std::basic_istream<char>& file_contents)
    -> std::optional<NodeConfig>
{
    namespace po = boost::program_options;
    po::options_description config_file_options("Config file options");
    std::string ip_address_str{};
    std::string qpid_broker_endpoint_string{};
    std::string library_root_str{};
    std::string node_uuid_str{};
    std::string re_bin_path{};
    std::string hostname_string{};

    config_file_options.add_options()("ip_address,i",
                                      po::value<std::string>(&ip_address_str)->required(),
                                      "node ip address");
    config_file_options.add_options()(
        "qpid_broker_endpoint,e", po::value<std::string>(&qpid_broker_endpoint_string)->required(),
        "Qpid broker endpoint eg 192.168.111.1:5672");
    config_file_options.add_options()("library_root,l",
                                      po::value<std::string>(&library_root_str)->required(),
                                      "Model library root directory");

    config_file_options.add_options()("uuid,u", po::value<std::string>(&node_uuid_str),
                                      "Node uuid");
    config_file_options.add_options()("host_name,h", po::value<std::string>(&hostname_string));
    config_file_options.add_options()("re_bin_path,p", po::value<std::string>(&re_bin_path)->required());

    try {
        po::variables_map parsed_options;
        po::store(po::parse_config_file(file_contents, config_file_options), parsed_options);
        po::notify(parsed_options);

        // If we don't have a uuid, a random one is initialised
        types::Uuid uuid{};
        if(parsed_options.count("uuid")) {
            // Read our uuid from our config file
            uuid = types::Uuid{node_uuid_str};
        }

        std::optional<std::string> hostname;
        if(hostname_string.empty()) {
            hostname = std::nullopt;
        } else {
            hostname = hostname_string;
        }

        return {{types::Ipv4::from_string(ip_address_str),
                 types::SocketAddress::from_string(qpid_broker_endpoint_string), library_root_str,
                 uuid, hostname, re_bin_path}};
    } catch(const std::exception& ex) {
        std::cout << config_file_options << std::endl;
        std::string error_message{"Error parsing config file: "};
        error_message += ex.what();
        throw std::invalid_argument(error_message);
    }
}

auto re::NodeManager::NodeConfig::SaveConfigFile(const NodeConfig& config) -> void
{
    std::ofstream file{config.file_path};
    if(file.is_open()) {
        file << config;
        file.close();
    } else {
        std::cerr << "Could not save node manager config file: " << std::endl;
        std::cerr << config;
        throw std::runtime_error("Could not save nodemanager config file!");
    }
}
auto re::NodeManager::NodeConfig::ParseArguments(int argc, char** argv) -> std::optional<std::string>
{
    namespace po = boost::program_options;
    po::options_description command_line_options("Node manager options");
    std::string config_file_path{};
    command_line_options.add_options()("config_file,f",
                                       boost::program_options::value<std::string>(&config_file_path)->required(),
                                       "Config file path");
    command_line_options.add_options()("help,h", "help");

    po::variables_map parsed_options;
    po::store(po::parse_command_line(argc, argv, command_line_options), parsed_options);
    po::notify(parsed_options);

    // If we encounter "help" in our args, print possible args and return nullopt.
    // This isn't a failure, so we don't throw. Onus is on caller to check that they got a
    //  populated optional
    if(parsed_options.count("help")) {
        std::cout << command_line_options << std::endl;
        return std::nullopt;
    }
    return config_file_path;
}
auto re::NodeManager::NodeConfig::HandleArguments(int argc, char** argv) -> std::optional<NodeConfig>
{
    auto config_file_path = ParseArguments(argc, argv);
    if(config_file_path) {
        std::ifstream config_file_stream(config_file_path.value());
        auto config = FromIstream(config_file_stream);
        config->file_path = config_file_path.value();
        return config;
    }
    return std::nullopt;
}