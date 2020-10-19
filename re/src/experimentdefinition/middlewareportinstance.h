#ifndef RE_MIDDLEWAREPORTINSTANCE_H
#define RE_MIDDLEWAREPORTINSTANCE_H
#include "modelentity.h"
#include <chrono>
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "uuid.h"
#include <vector>
namespace re::Representation {

class PortDelegateInstance;
class MiddlewarePortInstance : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::MiddlewarePortInstance;
    using MiddlewarePbType = re::network::protocol::experimentdefinition::PortMiddleware;
    MiddlewarePortInstance(GraphmlParser& parser,
                           const std::string& medea_id,
                           const types::Uuid& definition_uuid);
    enum class Middleware { Tao, Zmq, OsplDds, RtiDds, Qpid };
    explicit MiddlewarePortInstance(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

    auto AddConnectedMiddlewarePort(const MiddlewarePortInstance& other) -> void;
    auto AddConnectedPortDelegate(const PortDelegateInstance& delegate) -> void;
    [[nodiscard]] auto GetMiddleware() const -> Middleware { return middleware_; }
    [[nodiscard]] auto GetTopicName() const -> std::optional<std::string> { return topic_; }
    [[nodiscard]] auto GetDefinitionUuid() const -> types::Uuid { return definition_uuid_; }

private:
    re::types::Uuid definition_uuid_;
    Middleware middleware_;

    std::optional<std::string> topic_;

    std::vector<re::types::Uuid> internal_connected_port_uuids_;
    std::vector<re::types::Uuid> external_connected_port_uuids_;
    std::vector<re::types::Uuid> connected_port_delegate_uuids_;
};
} // namespace re::Representation

#endif // RE_MIDDLEWAREPORTINSTANCE_H
