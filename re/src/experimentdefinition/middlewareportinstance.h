#ifndef RE_MIDDLEWAREPORTINSTANCE_H
#define RE_MIDDLEWAREPORTINSTANCE_H
#include "modelentity.h"
#include <chrono>
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include "uuid.h"
#include <vector>
namespace re::Representation {

class MiddlewarePortInstance : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::MiddlewarePortInstance;
    using MiddlewarePbType = re::network::protocol::experimentdefinition::PortMiddleware;
    enum class Middleware { Tao, Zmq, OsplDds, RtiDds, Qpid };
    explicit MiddlewarePortInstance(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    re::types::Uuid definition_uuid_;
    Middleware middleware_;

    std::vector<re::types::Uuid> internal_connected_port_uuids_;
    std::vector<re::types::Uuid> external_connected_port_uuids_;
    std::vector<re::types::Uuid> connected_port_delegate_uuids_;
};
} // namespace re::Representation

#endif // RE_MIDDLEWAREPORTINSTANCE_H
