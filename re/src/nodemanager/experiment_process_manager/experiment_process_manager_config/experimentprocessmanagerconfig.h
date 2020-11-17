//
// Created by mitchell on 29/10/20.
//

#ifndef SEM_EXPERIMENTPROCESSMANAGERCONFIG_H
#define SEM_EXPERIMENTPROCESSMANAGERCONFIG_H

#include "socketaddress.hpp"
#include "uuid.h"
namespace sem::experiment_process_manager {

class ExperimentProcessManagerConfig {
public:
    types::Uuid epm_uuid;
    types::Uuid experiment_uuid;
    types::Uuid node_manager_uuid;
    types::Uuid container_uuid;
    types::Ipv4 control_ip_address;
    types::Ipv4 data_ip_address;
    types::SocketAddress process_registration_endpoint;
    types::SocketAddress experiment_manager_registration_endpoint;
    std::string lib_root_dir{};
    std::string re_bin_path{};

    static auto ParseArguments(int argc, char** argv) -> ExperimentProcessManagerConfig;
};
} // namespace sem::experiment_process_manager
#endif // SEM_EXPERIMENTPROCESSMANAGERCONFIG_H
