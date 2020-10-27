#ifndef SEM_EPMREGISTRY_H
#define SEM_EPMREGISTRY_H

#include "socketaddress.hpp"
#include "uuid.h"
namespace sem::node_manager {

class EpmRegistry {
public:
    struct EpmInfo {
        types::Uuid epm_uuid;
        types::SocketAddress control_endpoint;
        types::Ipv4 data_ip_address;
    };

    virtual auto start_epm() -> void = 0;
    virtual auto remove_epm(types::Uuid) -> void = 0;
    [[nodiscard]] virtual auto get_epm_info(types::Uuid) -> EpmInfo = 0;
    virtual ~EpmRegistry() = 0;
};

inline EpmRegistry::~EpmRegistry() = default;

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRY_H
