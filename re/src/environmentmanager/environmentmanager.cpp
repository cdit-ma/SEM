#include <boost/program_options.hpp>
#include <csignal>
#include <iostream>
#include <memory>
#include "uuid.h"

#include "sem_version.hpp"

#include "deploymentregister.h"

namespace {
volatile std::sig_atomic_t envmanager_signal_status = 0;
}
void signal_handler(int signal)
{
    envmanager_signal_status = signal;
}

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
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::string ip_address;
    uint16_t registration_port;
    std::string qpid_address;
    std::string tao_naming_service_address;

    // Parse Args
    boost::program_options::options_description options("Environment Manager Options");

    options.add_options()("ip_address,a",
                          boost::program_options::value<std::string>(&ip_address)->required(),
                          "Ip address of environment manager");
    options.add_options()("registration_port,r",
                          boost::program_options::value<uint16_t>(&registration_port)->required(),
                          "Port number for deployment registration.");

    options.add_options()("qpid_address,q",
                          boost::program_options::value<std::string>(&qpid_address)->required(),
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

    re::EnvironmentManager::DeploymentRegister dep_reg{
        sem::types::Ipv4(ip_address), registration_port,
        sem::types::SocketAddress(qpid_address), tao_naming_service_address};

    while(envmanager_signal_status == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
