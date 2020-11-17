#ifndef SEM_EPMREGISTRY_H
#define SEM_EPMREGISTRY_H

#include "nodemanagerconfig.h"
#include "socketaddress.hpp"
#include "uuid.h"
#include <boost/process/child.hpp>
namespace sem::node_manager {

class EpmRegistry {
public:
    struct EpmInfo {
        types::SocketAddress control_endpoint;
        types::Ipv4 data_ip_address;
    };

    using EpmProcessHandle = std::unique_ptr<boost::process::child>;

    struct EpmRegistrationResult {
        types::Uuid epm_uuid;
        EpmRegistry::EpmInfo epm_info;
        EpmRegistry::EpmProcessHandle process_handle;
    };

    struct EpmStartArguments {
        const NodeConfig node_config;
        const types::Uuid experiment_uuid;
        const types::Uuid container_uuid;

        [[nodiscard]] auto to_command_line_args() const -> std::string
        {
            auto epm_exe_path = node_config.find_epm_executable();
            return epm_exe_path + " --node_manager_uuid=" + node_config.uuid.to_string()
                   + " --experiment_uuid=" + experiment_uuid.to_string()
                   + " --experiment_manager_registration_endpoint="
                   + node_config.environment_manager_registration_endpoint.to_string()
                   + " --container_uuid=" + container_uuid.to_string()
                   + " --control_ip_address=" + node_config.control_ip_address.to_string()
                   + " --data_ip_address=" + node_config.data_ip_address.to_string()
                   + " --lib_root_dir=" + node_config.lib_root_dir
                   + " --re_bin_dir=" + node_config.re_bin_path;
        }
    };
    virtual auto
    start_epm(types::Uuid experiment_uuid, types::Uuid container_uuid) -> types::Uuid = 0;
    virtual auto remove_epm(types::Uuid) -> void = 0;
    [[nodiscard]] virtual auto get_epm_info(types::Uuid) -> EpmInfo = 0;
    virtual ~EpmRegistry() = 0;
};

inline EpmRegistry::~EpmRegistry() = default;

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRY_H
