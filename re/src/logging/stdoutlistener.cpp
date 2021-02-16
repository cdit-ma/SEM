#include "grpc_util/server.h"
#include "grpc_util/serverlifetimemanagerimpl.h"
#include "socketaddress.hpp"
#include "uuid.h"
#include <boost/program_options.hpp>

auto register_with_environment_manager(sem::types::Uuid experiment_uuid,
                                       sem::types::SocketAddress control_endpoint,
                                       sem::types::SocketAddress data_endpoint) -> void
{
}

auto main(int argc, char** argv) -> int
{
    using namespace sem::types;
    namespace po = boost::program_options;
    std::string experiment_uuid_string{};
    std::string registration_endpoint_string{};
    std::string control_ip_string{};
    std::string data_ip_string{};

    po::options_description options{"StdOutListener Options"};

    options.add_options()("registration_endpoint,e",
                          po::value<std::string>(&registration_endpoint_string)->required(),
                          "environment manager registration endpoint");
    options.add_options()("experiment_uuid,i",
                          po::value<std::string>(&experiment_uuid_string)->required(),
                          "experiment uuid to listen for");
    options.add_options()("control_ip_address,c",
                          po::value<std::string>(&control_ip_string)->required(),
                          "ip addr to use for control messages");
    options.add_options()("data_ip_address,d", po::value<std::string>(&data_ip_string)->required(),
                          "ip addr to use for data messages");

    po::variables_map parsed_options;
    po::store(po::parse_command_line(argc, argv, options), parsed_options);
    po::notify(parsed_options);

    auto experiment_uuid = Uuid{experiment_uuid_string};
    auto registration_endpoint = SocketAddress{registration_endpoint_string};
    auto control_address = Ipv4{control_ip_string};
    auto data_address = Ipv4{data_ip_string};

    using namespace sem::grpc_util;

    auto lifetime_manager = ServerLifetimeManagerImpl();

    // Pass lifetime manager to control service s.t. we can cause stdoutlistener shutdown remotely
    //auto control_service = EpmOutputControlServiceImpl(lifetime_manager);

    auto server = LifetimeManagedServer(lifetime_manager, {control_address, data_address},
                                        {
                                            /* TODO: epmoutputcontrol service */
                                            // only have the one service here, have control service own message handling service
                                        });

    auto control_endpoint = server.endpoints().at(control_address);
    auto data_endpoint = server.endpoints().at(data_address);
    register_with_environment_manager(experiment_uuid, control_endpoint, data_endpoint);

    lifetime_manager.wait();
}