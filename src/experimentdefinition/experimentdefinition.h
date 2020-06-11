#ifndef RE_EXPERIMENTDEFINITION_H
#define RE_EXPERIMENTDEFINITION_H

#include "attributedefinition.h"
#include "attributeinstance.h"
#include "cluster.h"
#include "componentassembly.h"
#include "componentdefinition.h"
#include "componentinstance.h"
#include "container.h"
#include "loggingclientdefinition.h"
#include "loggingserverdefinition.h"
#include "middlewareportdefinition.h"
#include "middlewareportinstance.h"
#include "node.h"
#include "periodicportdefinition.h"
#include "periodicportinstance.h"
#include "portdelegateinstance.h"
#include "strategy.h"
#include "triggerdefinition.h"
#include "triggerinstance.h"
#include "workerdefinition.h"
#include "workerinstance.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include <types/uuid.h>
namespace re::Representation {

class ExperimentDefinition {
public:
    using PbType = re::network::protocol::experimentdefinition::ExperimentDefinition;
    explicit ExperimentDefinition(const PbType& definition_pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    std::string experiment_name_;
    types::Uuid uuid_;

    std::unordered_map<types::Uuid, AttributeDefinition> attribute_definitions_;
    std::unordered_map<types::Uuid, std::unique_ptr<AttributeInstanceInterface>>
        attribute_instances_;

    // TODO: Add deployment attributes.

    std::unordered_map<types::Uuid, ComponentDefinition> component_definitions_;
    std::unordered_map<types::Uuid, ComponentInstance> component_instances_;

    std::unordered_map<types::Uuid, ComponentAssembly> component_assemblies_;
    std::unordered_map<types::Uuid, PortDelegateInstance> port_delegates_;

    std::unordered_map<types::Uuid, Container> container_definitions_;

    std::unordered_map<types::Uuid, MiddlewarePortDefinition> middleware_port_definitions_;
    std::unordered_map<types::Uuid, MiddlewarePortInstance> middleware_port_instances_;

    std::unordered_map<types::Uuid, PeriodicPortDefinition> periodic_port_definitions_;
    std::unordered_map<types::Uuid, PeriodicPortInstance> periodic_port_instances_;

    std::unordered_map<types::Uuid, WorkerDefinition> worker_definitions_;
    std::unordered_map<types::Uuid, WorkerInstance> worker_instances_;

    std::unordered_map<types::Uuid, Node> node_definitions_;
    std::unordered_map<types::Uuid, Cluster> cluster_definitions_;

    std::unordered_map<types::Uuid, TriggerDefinition> trigger_definitions_;
    std::unordered_map<types::Uuid, TriggerInstance> trigger_instances_;
    std::unordered_map<types::Uuid, Strategy> strategies_;

    std::unordered_map<types::Uuid, LoggingServerDefinition> logging_server_definitions_;
    std::unordered_map<types::Uuid, LoggingClientDefinition> logging_client_definitions_;
};

} // namespace re::Representation
#endif // RE_EXPERIMENTDEFINITION_H
