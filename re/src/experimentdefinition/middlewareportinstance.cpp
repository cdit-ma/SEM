#include "middlewareportinstance.h"
#include "periodicportinstance.h"

namespace re::Representation {
auto MiddlewareFromProto(const MiddlewarePortInstance::MiddlewarePbType& mw)
    -> MiddlewarePortInstance::Middleware
{
    switch(mw) {
        case network::protocol::experimentdefinition::Tao:
            return MiddlewarePortInstance::Middleware::Tao;
        case network::protocol::experimentdefinition::Zmq:
            return MiddlewarePortInstance::Middleware::Zmq;
        case network::protocol::experimentdefinition::RtiDds:
            return MiddlewarePortInstance::Middleware::RtiDds;
        case network::protocol::experimentdefinition::OsplDds:
            return MiddlewarePortInstance::Middleware::OsplDds;
        case network::protocol::experimentdefinition::Qpid:
            return MiddlewarePortInstance::Middleware::Qpid;
        case network::protocol::experimentdefinition::PortMiddleware_INT_MIN_SENTINEL_DO_NOT_USE_:
            [[fallthrough]];
        case network::protocol::experimentdefinition::PortMiddleware_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    throw std::invalid_argument("Invalid middleware found in experimentdefinition::port proto -> "
                                "middleware");
}

auto MiddlewareToProto(const MiddlewarePortInstance::Middleware& mw)
    -> MiddlewarePortInstance::MiddlewarePbType
{
    switch(mw) {
        case MiddlewarePortInstance::Middleware::Tao:
            return network::protocol::experimentdefinition::Tao;
        case MiddlewarePortInstance::Middleware::Zmq:
            return network::protocol::experimentdefinition::Zmq;
        case MiddlewarePortInstance::Middleware::RtiDds:
            return network::protocol::experimentdefinition::RtiDds;
        case MiddlewarePortInstance::Middleware::OsplDds:
            return network::protocol::experimentdefinition::OsplDds;
        case MiddlewarePortInstance::Middleware::Qpid:
            return network::protocol::experimentdefinition::Qpid;
    }
    throw std::invalid_argument("Invalid middleware found in experimentdefinition::port middleware "
                                "-> proto");
}
MiddlewarePortInstance::MiddlewarePortInstance(const MiddlewarePortInstance::PbType& pb) :
    DefaultModelEntity{pb.core_data()},
    definition_uuid_{pb.definition_uuid()},
    middleware_{MiddlewareFromProto(pb.middleware())}
{
    for(const auto& external_connected_port_uuid : pb.external_connected_port_uuids()) {
        external_connected_port_uuids_.emplace_back(external_connected_port_uuid);
    }

    for(const auto& internal_connected_port_uuid : pb.internal_connected_port_uuids()) {
        internal_connected_port_uuids_.emplace_back(internal_connected_port_uuid);
    }

    for(const auto& connected_port_delegate_uuid : pb.connected_port_delegate_uuids()) {
        connected_port_delegate_uuids_.emplace_back(connected_port_delegate_uuid);
    }
}
auto MiddlewarePortInstance::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_definition_uuid(definition_uuid_.to_string());
    out->set_middleware(MiddlewareToProto(middleware_));

    for(const auto& port_uuid : internal_connected_port_uuids_) {
        out->add_internal_connected_port_uuids(port_uuid.to_string());
    }

    for(const auto& port_uuid : external_connected_port_uuids_) {
        out->add_external_connected_port_uuids(port_uuid.to_string());
    }

    for(const auto& delegate_uuid : connected_port_delegate_uuids_) {
        out->add_connected_port_delegate_uuids(delegate_uuid.to_string());
    }
    return out;
}

} // namespace re::Representation