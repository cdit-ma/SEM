#include <boost/program_options.hpp>
#include "socketaddress.hpp"
#include "uuid.h"

auto register_with_environment_manager() -> void {

}

auto main(int argc, char** argv) -> int
{
    namespace po = boost::program_options;
    std::string experiment_uuid_string{};
    std::string registration_endpoint_string{};

    po::options_description options{"StdOutListener Options"};

    options.add_options()("registration_endpoint,e",
                          po::value<std::string>(&registration_endpoint_string)->required(),
                          "environment manager registration endpoint");
    options.add_options()("experiment_uuid,i",
                          po::value<std::string>(&experiment_uuid_string)->required(),
                          "experiment uuid to listen for");
    po::variables_map parsed_options;
    po::store(po::parse_command_line(argc, argv, options), parsed_options);
    po::notify(parsed_options);


    auto uuid = sem::types::Uuid{experiment_uuid_string};
    auto registration_endpoint = sem::types::SocketAddress(registration_endpoint_string);



    register_with_environment_manager(control_endpoint, data_endpoint);



}