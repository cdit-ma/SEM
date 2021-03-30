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
#include "experimentdefinition.pb.h"
#include "uuid.h"
namespace re::Representation {

class ExperimentDefinition {
public:
    using PbType = re::network::protocol::experimentdefinition::ExperimentDefinition;
    explicit ExperimentDefinition(const PbType& definition_pb);
    explicit ExperimentDefinition(std::istream& model);

    auto GetUuid() -> sem::types::Uuid;

    auto SetName(const std::string& experiment_name) -> void;
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

    auto GetContainerToStartOnTriggerEvent(const std::string& trigger_medea_id) -> Container;
    auto GetDeploymentLocation(const Container& container) const -> std::variant<Node, Cluster>;
    auto
    GetContainersComponents(const Container& container) const -> std::vector<ComponentInstance>;
    auto GetComponentInstanceDefinition(const ComponentInstance& component_instance) const
        -> ComponentDefinition;
    auto GetComponentInstanceMiddlewarePorts(const ComponentInstance& component_instance) const
        -> std::vector<MiddlewarePortInstance>;

    auto GetComponentInstancePeriodicPorts(const ComponentInstance& component_instance) const
        -> std::vector<PeriodicPortInstance>;

    auto GetPeriodicPortFrequencyAttribute(const PeriodicPortInstance& periodic_port_instance) const
        -> double;
    auto GetMiddlewarePortDefinition(const MiddlewarePortInstance& instance) const
        -> MiddlewarePortDefinition;

    auto GetTriggerInstFromMedeaId(const std::string& trigger_inst_id) const -> TriggerInstance;
    auto GetTriggerDefinition(const TriggerInstance& instance) const -> TriggerDefinition;
    auto GetComponentInstanceWorkers(const ComponentInstance& instance) const
        -> std::vector<WorkerInstance>;

    auto GetWorkerDefinition(const WorkerInstance& inst) const -> WorkerDefinition;

    auto GetComponentAttributeInstances(const ComponentInstance& component) const
        -> std::vector<std::unique_ptr<AttributeInstancePb>>;

    auto dbg() const -> void { ToProto()->PrintDebugString(); };

private:
    std::string experiment_name_;
    sem::types::Uuid uuid_;

    std::unordered_map<sem::types::Uuid, AttributeDefinition> attribute_definitions_;
    std::unordered_map<sem::types::Uuid, std::unique_ptr<AttributeInstanceInterface>>
        attribute_instances_;

    // TODO: Add deployment attributes.

    std::unordered_map<sem::types::Uuid, ComponentDefinition> component_definitions_;
    std::unordered_map<sem::types::Uuid, ComponentInstance> component_instances_;

    std::unordered_map<sem::types::Uuid, ComponentAssembly> component_assemblies_;
    std::unordered_map<sem::types::Uuid, PortDelegateInstance> port_delegates_;

    std::unordered_map<sem::types::Uuid, Container> container_definitions_;

    std::unordered_map<sem::types::Uuid, MiddlewarePortDefinition> middleware_port_definitions_;
    std::unordered_map<sem::types::Uuid, MiddlewarePortInstance> middleware_port_instances_;

    std::unordered_map<sem::types::Uuid, PeriodicPortDefinition> periodic_port_definitions_;
    std::unordered_map<sem::types::Uuid, PeriodicPortInstance> periodic_port_instances_;

    std::unordered_map<sem::types::Uuid, WorkerDefinition> worker_definitions_;
    std::unordered_map<sem::types::Uuid, WorkerInstance> worker_instances_;

    std::unordered_map<sem::types::Uuid, Node> node_definitions_;
    std::unordered_map<sem::types::Uuid, Cluster> cluster_definitions_;

    std::unordered_map<sem::types::Uuid, TriggerDefinition> trigger_definitions_;
    std::unordered_map<sem::types::Uuid, TriggerInstance> trigger_instances_;
    std::unordered_map<sem::types::Uuid, Strategy> strategies_;

    std::unordered_map<sem::types::Uuid, LoggingServerDefinition> logging_server_definitions_;
    std::unordered_map<sem::types::Uuid, LoggingClientDefinition> logging_client_definitions_;

    std::unordered_map<sem::types::Uuid, std::string> uuid_to_medea_id_map_;
    void RegisterMedeaId(const sem::types::Uuid& uuid, const std::string& medea_id);
    auto GetUuidFromMedeaId(const std::string& medea_id) -> sem::types::Uuid;

    auto PopulateClusters(GraphmlParser& graphml_parser) -> void;
    void PopulateNodes(GraphmlParser& graphml_parser);
    auto AddNodeToCluster(GraphmlParser& graphml_parser, const Node& node) -> void;

    void PopulateContainers(GraphmlParser& parser);
    void DeployContainers(GraphmlParser& parser);

    void PopulateLoggingServers(GraphmlParser& parser);
    void PopulateLoggingClients(GraphmlParser& parser);
    std::vector<sem::types::Uuid>
    GetDeploymentLocations(GraphmlParser& parser, const std::string& deployed_entity_medea_id);
    void ConnectLoggingClientsToServers(GraphmlParser& parser);
    void PopulateComponents(GraphmlParser& parser);
    void PopulateMiddlewarePorts(GraphmlParser& parser);
    void PopulateAttributes(GraphmlParser& parser);
    void PopulatePortDelegates(GraphmlParser& parser);
    void ConnectPortsAndPortDelegates(GraphmlParser& parser);
    void PopulatePeriodicPorts(GraphmlParser& parser);
    void PopulateComponentAssemblies(GraphmlParser& parser);
    void PopulateWorkers(GraphmlParser& parser);
    void DeployComponentInstances(GraphmlParser& parser);
    void DeployComponentAssemblies(GraphmlParser& parser);
    void PopulateTriggers(GraphmlParser& parser);
    void PopulateStrategies(GraphmlParser& parser);
};

} // namespace re::Representation
#endif // RE_EXPERIMENTDEFINITION_H
