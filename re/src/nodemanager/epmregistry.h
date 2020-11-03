#ifndef SEM_EPMREGISTRY_H
#define SEM_EPMREGISTRY_H

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

    virtual auto start_epm(types::Uuid experiment_uuid, types::Uuid container_uuid) -> types::Uuid = 0;
    virtual auto remove_epm(types::Uuid) -> void = 0;
    [[nodiscard]] virtual auto get_epm_info(types::Uuid) -> EpmInfo = 0;
    virtual ~EpmRegistry() = 0;
};

inline EpmRegistry::~EpmRegistry() = default;

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRY_H
