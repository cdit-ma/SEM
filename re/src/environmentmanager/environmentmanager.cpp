#include <boost/program_options.hpp>
#include <iostream>

#include "sem_version.hpp"

#include "environmentcontrol.h"
#include "nodemanagerregistrarimpl.h"

#include "experimentregistry.h"
#include "grpc_util/server.h"
#include "socketaddress.hpp"

/**
 * Environment manager main function.
 *
 * Args:
 *  * ip_address - Ip address that environment manager should use for zmq based comms with
 *      experiment components.
 *  * qpid_address - Qpid broker endpoint to use. In format 192.168.111.123:5672. [required]
 *  * tao_naming_service_address - Tao naming endpoint to use. [optional]
 *  * registration_port - Port used to communicate with the environment manager. [required]
 */
int main(int argc, char** argv)
{
    std::string ip_address_str;
    uint16_t registration_port;
    uint16_t control_port;
    std::string qpid_address;
    std::string tao_naming_service_address;

    // Parse Args
    boost::program_options::options_description options("Environment Manager Options");

    options.add_options()("ip_address,a",
                          boost::program_options::value<std::string>(&ip_address_str)->required(),
                          "Ip address of environment manager");
    options.add_options()("registration_port,r",
                          boost::program_options::value<uint16_t>(&registration_port)->required(),
                          "Port number for deployment registration.");
    options.add_options()("control_port,c",
                          boost::program_options::value<uint16_t>(&control_port)->required(),
                          "Port number for environment control.");

    options.add_options()("qpid_address,q",
                          boost::program_options::value<std::string>(&qpid_address),
                          "Endpoint of qpid broker. <ip_addr:port>");

    options.add_options()("tao_naming_service_address,t",
                          boost::program_options::value<std::string>(&tao_naming_service_address),
                          "Endpoint of tao naming service.");
    options.add_options()("help,h", "Display help");

    boost::program_options::variables_map vm;

    try {
        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, options), vm);
        boost::program_options::notify(vm);
    } catch(...) {
        std::cout << options << std::endl;
        return 1;
    }

    if(vm.count("help")) {
        std::cerr << options << std::endl;
        return 0;
    }

    if(tao_naming_service_address.empty()) {
        std::cerr << "* No Tao naming service address set. This will cause models containing TAO "
                     "ports to fail."
                  << std::endl;
    }

    using namespace sem::environment_manager;

    sem::types::Ipv4 ip_address{ip_address_str};
    sem::types::SocketAddress qpid_broker_address{qpid_address};


    re::EnvironmentManager::Environment environment(ip_address, qpid_address, "", );
    std::unique_ptr<NodeManagerRegistry> nm_registry = std::make_unique<NodeManagerRegistryImpl>();
    std::unique_ptr<ExperimentRegistry> experiment_registry = std::make_unique<ExperimentRegistry>();

    auto nm_registrar = std::make_shared<NodeManagerRegistrarImpl>(*nm_registry);
    auto environment_control = std::make_shared<EnvironmentControl>(*nm_registry, &experiment_registry);

    sem::grpc_util::Server environment_manager_server{
        sem::types::SocketAddress{sem::types::Ipv4{ip_address}, registration_port},
        {nm_registrar, environment_control}};

    // TODO: Do ctrl-c handling right...
    environment_manager_server.wait();

    return 0;
}
