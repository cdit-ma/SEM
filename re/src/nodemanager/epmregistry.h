#ifndef SEM_EPMREGISTRY_H
#define SEM_EPMREGISTRY_H

#include "socketaddress.hpp"
#include "uuid.h"
namespace sem::node_manager {

class EpmRegistry {
public:
    struct EpmInfo {
        types::SocketAddress data_endpoint_;
        types::SocketAddress control_endpoint_;
    };

    virtual auto start_epm() -> void = 0;
    virtual auto epm_started(types::Uuid,
                             types::SocketAddress control_endpoint,
                             types::SocketAddress data_endpoint) -> void = 0;
    virtual auto remove_epm(types::Uuid) -> void = 0;
    [[nodiscard]] virtual auto get_epm_info(types::Uuid) -> EpmInfo = 0;
    virtual ~EpmRegistry() = 0;
};

inline EpmRegistry::~EpmRegistry() = default;

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRY_H
