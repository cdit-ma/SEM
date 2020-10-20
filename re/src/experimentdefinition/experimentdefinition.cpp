#include "experimentdefinition.h"
namespace re::Representation {

namespace detail {

// Review: Thoughts on a singleton here?
auto GetDeploymentMap(GraphmlParser& graphml_parser)
    -> std::unordered_map<std::string, std::vector<std::string>>
{
    auto deployment_edge_ids = graphml_parser.FindEdges("Edge_Deployment");
    std::unordered_map<std::string, std::vector<std::string>> deployment_edges;
    for(const auto& edge_id : deployment_edge_ids) {
        deployment_edges[graphml_parser.GetAttribute(edge_id, "source")].emplace_back(
            graphml_parser.GetAttribute(edge_id, "target"));
    }
    return deployment_edges;
}

// Returns map pointing from instance to definition
// instance_id -> definition_id
auto GetDefinitionMap(GraphmlParser& graphml_parser) -> std::unordered_map<std::string, std::string>
{
    auto deployment_edge_ids = graphml_parser.FindEdges("Edge_Definition");
    std::unordered_map<std::string, std::string> definition_edges;
    for(const auto& edge_id : deployment_edge_ids) {
        definition_edges[graphml_parser.GetAttribute(edge_id, "source")] =
            graphml_parser.GetAttribute(edge_id, "target");
    }
    return definition_edges;
}

auto GetAssemblyEdges(GraphmlParser& graphml_parser)
    -> std::vector<std::pair<std::string, std::string>>
{
    std::vector<std::pair<std::string, std::string>> edges;
    auto assembly_edge_ids = graphml_parser.FindEdges("Edge_Assembly");
    for(const auto& edge_id : assembly_edge_ids) {
        edges.emplace_back(graphml_parser.GetAttribute(edge_id, "source"),
                           graphml_parser.GetAttribute(edge_id, "target"));
    }
    return edges;
}

} // namespace detail

auto ExperimentDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_uuid(uuid_.to_string());
    out->set_experiment_name(experiment_name_);

    auto& attribute_map = *out->mutable_attribute_definitions();
    for(const auto& [uuid, attribute_definition] : attribute_definitions_) {
        attribute_map[uuid.to_string()] = *attribute_definition.ToProto();
    }

    auto& attr_instance_map = *out->mutable_attribute_instances();
    for(const auto& [uuid, attribute_instance] : attribute_instances_) {
        attr_instance_map[uuid.to_string()] = *attribute_instance->ToProto();
    }

    auto& component_map = *out->mutable_component_definitions();
    for(const auto& [uuid, component] : component_definitions_) {
        component_map[uuid.to_string()] = *component.ToProto();
    }

    auto& component_instance_map = *out->mutable_component_instances();
    for(const auto& [uuid, component_instance] : component_instances_) {
        component_instance_map[uuid.to_string()] = *component_instance.ToProto();
    }

    auto& assembly_map = *out->mutable_component_assemblies();
    for(const auto& [uuid, assembly] : component_assemblies_) {
        assembly_map[uuid.to_string()] = *assembly.ToProto();
    }

    auto& delegate_map = *out->mutable_port_delegates();
    for(const auto& [uuid, delegate] : port_delegates_) {
        delegate_map[uuid.to_string()] = *delegate.ToProto();
    }

    auto& container_map = *out->mutable_containers();
    for(const auto& [uuid, container] : container_definitions_) {
        container_map[uuid.to_string()] = *container.ToProto();
    }

    auto& mw_port_def_map = *out->mutable_middleware_port_definitions();
    for(const auto& [uuid, mw_port_def] : middleware_port_definitions_) {
        mw_port_def_map[uuid.to_string()] = *mw_port_def.ToProto();
    }

    auto& mw_port_inst_map = *out->mutable_middleware_port_instances();
    for(const auto& [uuid, mw_port_inst] : middleware_port_instances_) {
        mw_port_inst_map[uuid.to_string()] = *mw_port_inst.ToProto();
    }

    auto& periodic_port_def_map = *out->mutable_periodic_port_definitions();
    for(const auto& [uuid, periodic_port_def] : periodic_port_definitions_) {
        periodic_port_def_map[uuid.to_string()] = *periodic_port_def.ToProto();
    }

    auto& periodic_port_inst_map = *out->mutable_periodic_port_instances();
    for(const auto& [uuid, port_inst] : periodic_port_instances_) {
        periodic_port_inst_map[uuid.to_string()] = *port_inst.ToProto();
    }

    auto& worker_definitions_map = *out->mutable_worker_definitions();
    for(const auto& [uuid, worker_definition] : worker_definitions_) {
        worker_definitions_map[uuid.to_string()] = *worker_definition.ToProto();
    }

    auto& worker_instances_map = *out->mutable_worker_instances();
    for(const auto& [uuid, worker_instance] : worker_instances_) {
        worker_instances_map[uuid.to_string()] = *worker_instance.ToProto();
    }

    auto& node_defs_map = *out->mutable_nodes();
    for(const auto& [uuid, node] : node_definitions_) {
        node_defs_map[uuid.to_string()] = *node.ToProto();
    }
    auto& cluster_defs_map = *out->mutable_clusters();
    for(const auto& [uuid, cluster] : cluster_definitions_) {
        cluster_defs_map[uuid.to_string()] = *cluster.ToProto();
    }

    auto& trigger_defs_map = *out->mutable_trigger_definitions();
    for(const auto& [uuid, trigger] : trigger_definitions_) {
        trigger_defs_map[uuid.to_string()] = *trigger.ToProto();
    }

    auto& trigger_inst_map = *out->mutable_trigger_instances();
    for(const auto& [uuid, trigger] : trigger_instances_) {
        trigger_inst_map[uuid.to_string()] = *trigger.ToProto();
    }
    auto& strategy_map = *out->mutable_strategies();
    for(const auto& [uuid, strategy] : strategies_) {
        strategy_map[uuid.to_string()] = *strategy.ToProto();
    }

    auto& logging_client_map = *out->mutable_logging_client_definitions();
    for(const auto& [uuid, logging_client] : logging_client_definitions_) {
        logging_client_map[uuid.to_string()] = *logging_client.ToProto();
    }

    auto& logging_server_map = *out->mutable_logging_server_definitions();
    for(const auto& [uuid, logging_server] : logging_server_definitions_) {
        logging_server_map[uuid.to_string()] = *logging_server.ToProto();
    }

    auto& worker_definition_map = *out->mutable_worker_definitions();
    for(const auto& [uuid, worker_def] : worker_definitions_) {
        worker_definition_map[uuid.to_string()] = *worker_def.ToProto();
    }

    auto& worker_instance_map = *out->mutable_worker_instances();
    for(const auto& [uuid, worker_inst] : worker_instances_) {
        worker_instance_map[uuid.to_string()] = *worker_inst.ToProto();
    }

    return out;
}

ExperimentDefinition::ExperimentDefinition(const ExperimentDefinition::PbType& definition_pb)
{
    experiment_name_ = definition_pb.experiment_name();
    uuid_ = sem::types::Uuid{definition_pb.uuid()};

    for(const auto& [uuid, attribute_definition] : definition_pb.attribute_definitions()) {
        attribute_definitions_.insert(
            {sem::types::Uuid(uuid), AttributeDefinition{attribute_definition}});
    }

    for(const auto& [uuid, attribute_instance] : definition_pb.attribute_instances()) {
        attribute_instances_.insert(
            {sem::types::Uuid{uuid}, ConstructAttributeInstance(attribute_instance)});
    }

    for(const auto& [uuid, component_definition] : definition_pb.component_definitions()) {
        component_definitions_.insert(
            {sem::types::Uuid{uuid}, ComponentDefinition{component_definition}});
    }

    for(const auto& [uuid, component_instance] : definition_pb.component_instances()) {
        component_instances_.insert({sem::types::Uuid{uuid}, ComponentInstance{component_instance}});
    }

    for(const auto& [uuid, component_assembly] : definition_pb.component_assemblies()) {
        component_assemblies_.insert({sem::types::Uuid{uuid}, ComponentAssembly{component_assembly}});
    }

    for(const auto& [uuid, container] : definition_pb.containers()) {
        container_definitions_.insert({sem::types::Uuid{uuid}, Container{container}});
    }

    for(const auto& [uuid, middleware_port_definition] :
        definition_pb.middleware_port_definitions()) {
        middleware_port_definitions_.insert(
            {sem::types::Uuid{uuid}, MiddlewarePortDefinition{middleware_port_definition}});
    }

    for(const auto& [uuid, middleware_port_instance] : definition_pb.middleware_port_instances()) {
        middleware_port_instances_.insert(
            {sem::types::Uuid{uuid}, MiddlewarePortInstance{middleware_port_instance}});
    }

    for(const auto& [uuid, periodic_port_definition] : definition_pb.periodic_port_definitions()) {
        periodic_port_definitions_.insert(
            {sem::types::Uuid{uuid}, PeriodicPortDefinition{periodic_port_definition}});
    }

    for(const auto& [uuid, periodic_port_instance] : definition_pb.periodic_port_instances()) {
        periodic_port_instances_.insert(
            {sem::types::Uuid{uuid}, PeriodicPortInstance{periodic_port_instance}});
    }

    for(const auto& [uuid, port_delegate] : definition_pb.port_delegates()) {
        port_delegates_.insert({sem::types::Uuid{uuid}, PortDelegateInstance{port_delegate}});
    }

    for(const auto& [uuid, node] : definition_pb.nodes()) {
        node_definitions_.insert({sem::types::Uuid{uuid}, Node{node}});
    }

    for(const auto& [uuid, cluster] : definition_pb.clusters()) {
        cluster_definitions_.insert({sem::types::Uuid{uuid}, Cluster{cluster}});
    }

    for(const auto& [uuid, trigger_definition] : definition_pb.trigger_definitions()) {
        trigger_definitions_.insert({sem::types::Uuid{uuid}, TriggerDefinition{trigger_definition}});
    }

    for(const auto& [uuid, trigger_instance] : definition_pb.trigger_instances()) {
        trigger_instances_.insert({sem::types::Uuid{uuid}, TriggerInstance{trigger_instance}});
    }

    for(const auto& [uuid, strategy] : definition_pb.strategies()) {
        strategies_.insert({sem::types::Uuid{uuid}, Strategy{strategy}});
    }

    for(const auto& [uuid, logging_server] : definition_pb.logging_server_definitions()) {
        logging_server_definitions_.insert(
            {sem::types::Uuid{uuid}, LoggingServerDefinition{logging_server}});
    }

    for(const auto& [uuid, logging_client] : definition_pb.logging_client_definitions()) {
        logging_client_definitions_.insert(
            {sem::types::Uuid{uuid}, LoggingClientDefinition{logging_client}});
    }
}

ExperimentDefinition::ExperimentDefinition(std::istream& graphml_stream)
{
    GraphmlParser graphml_parser{graphml_stream};

    PopulateAttributes(graphml_parser);

    PopulateClusters(graphml_parser);
    PopulateNodes(graphml_parser);
    PopulateContainers(graphml_parser);
    DeployContainers(graphml_parser);

    PopulateLoggingServers(graphml_parser);
    PopulateLoggingClients(graphml_parser);
    ConnectLoggingClientsToServers(graphml_parser);

    PopulateComponents(graphml_parser);

    PopulateMiddlewarePorts(graphml_parser);
    PopulatePeriodicPorts(graphml_parser);
    PopulatePortDelegates(graphml_parser);

    ConnectPortsAndPortDelegates(graphml_parser);

    PopulateComponentAssemblies(graphml_parser);

    DeployComponentAssemblies(graphml_parser);
    DeployComponentInstances(graphml_parser);

    // TODO: DeploymentAttributes

    PopulateWorkers(graphml_parser);
    PopulateTriggers(graphml_parser);
    PopulateStrategies(graphml_parser);
}

auto ExperimentDefinition::PopulateNodes(GraphmlParser& graphml_parser) -> void
{
    auto hardware_node_ids = graphml_parser.FindNodesOfKind("HardwareNode");
    for(const auto& hardware_node_id : hardware_node_ids) {
        try {
            Node node{graphml_parser, hardware_node_id};
            if(node.GetCoreData().GetMedeaName() == "localhost") {
                // XXX: Skip localhost node for the time being
                continue;
            }

            AddNodeToCluster(graphml_parser, node);

            RegisterMedeaId(node.GetCoreData().GetUuid(), hardware_node_id);

            node_definitions_.emplace(node.GetCoreData().GetUuid(), node);
        } catch(const std::invalid_argument& ex) {
            // Found node that's offline, do nothing and move onto the next
            //  Report that we found one though, no-one likes silent catch blocks
            std::cerr << "Found offline node in graphml! Node ID: " << hardware_node_id
                      << std::endl;
            std::cerr << ex.what() << std::endl;
        }
    }
}

// Populate cluster definitions. Does not populate clusters owned nodes, components, component
//  assemblies, containers or loggers
void ExperimentDefinition::PopulateClusters(GraphmlParser& graphml_parser)
{
    auto hardware_cluster_ids = graphml_parser.FindNodesOfKind("HardwareCluster");
    for(const auto& cluster_id : hardware_cluster_ids) {
        Cluster cluster{graphml_parser, cluster_id};
        RegisterMedeaId(cluster.GetCoreData().GetUuid(), cluster_id);
        cluster_definitions_.emplace(cluster.GetCoreData().GetUuid(), cluster);
    }
}

// Add a node to it's parent cluster
// Precondition: cluster_definitions_ map is populated
auto ExperimentDefinition::AddNodeToCluster(GraphmlParser& graphml_parser, const Node& node) -> void
{
    auto cluster_medea_id = graphml_parser.GetParentNode(node.GetCoreData().GetMedeaId());
    auto cluster_uuid = GetUuidFromMedeaId(cluster_medea_id);
    try {
        cluster_definitions_.at(cluster_uuid).AddNode(node);
    } catch(const std::out_of_range& ex) {
        throw std::out_of_range("Could not find node's parent cluster. Node id: "
                                + node.GetCoreData().GetMedeaId());
    }
}

// Populate containers
void ExperimentDefinition::PopulateContainers(GraphmlParser& parser)
{
    auto deployment_container_ids = parser.FindNodesOfKind("DeploymentContainer");
    for(const auto& container_id : deployment_container_ids) {
        Container container{parser, container_id};
        RegisterMedeaId(container.GetCoreData().GetUuid(), container_id);
        container_definitions_.emplace(container.GetCoreData().GetUuid(), container);
    }
}

void ExperimentDefinition::PopulateLoggingServers(GraphmlParser& parser)
{
    auto logging_server_ids = parser.FindNodesOfKind("LoggingServer");
    for(const auto& logging_server_id : logging_server_ids) {
        LoggingServerDefinition logging_server{parser, logging_server_id};
        RegisterMedeaId(logging_server.GetCoreData().GetUuid(), logging_server_id);

        auto deployment_locations = GetDeploymentLocations(parser, logging_server_id);

        for(const auto& deployment_location_uuid : deployment_locations) {
            if(node_definitions_.count(deployment_location_uuid) != 0) {
                node_definitions_.at(deployment_location_uuid)
                    .AddDeployedLoggingServer(logging_server);
            }
        }
        logging_server_definitions_.emplace(logging_server.GetCoreData().GetUuid(), logging_server);
    }
}

void ExperimentDefinition::PopulateLoggingClients(GraphmlParser& parser)
{
    auto logging_client_ids = parser.FindNodesOfKind("LoggingProfile");
    for(const auto& logging_client_id : logging_client_ids) {
        LoggingClientDefinition logging_client_definition{parser, logging_client_id};
        RegisterMedeaId(logging_client_definition.GetCoreData().GetUuid(), logging_client_id);
        auto deployment_locations = GetDeploymentLocations(parser, logging_client_id);

        for(const auto& deployment_location_uuid : deployment_locations) {
            if(node_definitions_.count(deployment_location_uuid) != 0) {
                node_definitions_.at(deployment_location_uuid)
                    .AddDeployedLoggingClient(logging_client_definition);
            }
            if(container_definitions_.count(deployment_location_uuid) != 0) {
                container_definitions_.at(deployment_location_uuid)
                    .AddDeployedLoggingClient(logging_client_definition);
            }
            if(cluster_definitions_.count(deployment_location_uuid) != 0) {
                cluster_definitions_.at(deployment_location_uuid)
                    .AddDeployedLoggingClient(logging_client_definition);
            }
        }
        logging_client_definitions_.emplace(logging_client_definition.GetCoreData().GetUuid(),
                                            logging_client_definition);
    }
}

// Deploys containers to Nodes and clusters
// Preconditions: node_definitions_ and cluster_definitions_ must be populated.
void ExperimentDefinition::DeployContainers(GraphmlParser& parser)
{
    for(const auto& [uuid, container] : container_definitions_) {
        const auto&& container_medea_id = container.GetCoreData().GetMedeaId();

        auto deployment_location_uuids = GetDeploymentLocations(parser, container_medea_id);
        if(deployment_location_uuids.empty()) {
            std::cerr << "Container not deployed: " << container_medea_id << " : "
                      << container.GetCoreData().GetMedeaName() << std::endl;
            continue;
        } else {
            auto deployment_location_uuid = deployment_location_uuids.front();
            if(node_definitions_.count(deployment_location_uuid) != 0) {
                node_definitions_.at(deployment_location_uuid).AddDeployedContainer(container);
            } else if(cluster_definitions_.count(deployment_location_uuid) != 0) {
                cluster_definitions_.at(deployment_location_uuid).AddDeployedContainer(container);
            } else {
                std::cerr << "Could not find node container should be deployed to" << std::endl;
                throw std::runtime_error("Could not find node or cluster container is meant to be "
                                         "deployed to");
            }
        }
    }
}

// Adds logging client uuids to their connected servers.
// Preconditions: logging_client_definitions_ and logging_server_definitions_ must be populated.
//                uuid_to_medea_id map must also contain both client and server ids
void ExperimentDefinition::ConnectLoggingClientsToServers(GraphmlParser& parser)
{
    auto assembly_edge_ids = parser.FindEdges("Edge_Assembly");
    for(const auto& edge_id : assembly_edge_ids) {
        auto target_id = parser.GetAttribute(edge_id, "target");
        auto source_id = parser.GetAttribute(edge_id, "source");

        sem::types::Uuid client_uuid;
        sem::types::Uuid server_uuid;
        try {
            client_uuid = GetUuidFromMedeaId(source_id);
            server_uuid = GetUuidFromMedeaId(target_id);
        } catch(const std::out_of_range& ex) {
            // Due to logging clients/servers using Assembly edges for both connection and
            //  deployment, there are situations were either source or target may not be registered
            //  yet.
            continue;
        }
        if(logging_server_definitions_.count(server_uuid) != 0
           && logging_client_definitions_.count(client_uuid) != 0) {
            // Check that we're trying to link a logging client and server. Not anything else
            //  that uses assembly edges.
            logging_server_definitions_.at(server_uuid)
                .AddConnectedLoggingClient(logging_client_definitions_.at(client_uuid));
        }
    }
}

// Throws
void ExperimentDefinition::RegisterMedeaId(const sem::types::Uuid& uuid, const std::string& medea_id)
{
    if(uuid_to_medea_id_map_.count(uuid)) {
        throw std::invalid_argument("UUID collision when registering medea id: " + medea_id);
    }
    uuid_to_medea_id_map_[uuid] = medea_id;
}

auto ExperimentDefinition::GetUuidFromMedeaId(const std::string& medea_id) -> sem::types::Uuid
{
    auto iter = std::find_if(uuid_to_medea_id_map_.begin(), uuid_to_medea_id_map_.end(),
                             [medea_id](auto pair) { return pair.second == medea_id; });
    if(iter != uuid_to_medea_id_map_.end()) {
        return iter->first;
    }
    throw std::out_of_range("Could not find medea id in medea_id to uuid map: " + medea_id);
}

auto ExperimentDefinition::GetDeploymentLocations(GraphmlParser& parser,
                                                  const std::string& deployed_entity_medea_id)
    -> std::vector<sem::types::Uuid>
{
    auto deployment_map = detail::GetDeploymentMap(parser);

    auto deployment_locations_medea = deployment_map.at(deployed_entity_medea_id);
    std::vector<sem::types::Uuid> out{};
    for(const auto& medea_id : deployment_locations_medea) {
        out.emplace_back(GetUuidFromMedeaId(medea_id));
    }
    return out;
}

void ExperimentDefinition::PopulateComponents(GraphmlParser& parser)
{
    auto component_ids_ = parser.FindNodesOfKind("Component");

    for(const auto& component_id : component_ids_) {
        ComponentDefinition component_definition{parser, component_id};
        RegisterMedeaId(component_definition.GetCoreData().GetUuid(), component_id);
        component_definitions_.emplace(component_definition.GetCoreData().GetUuid(),
                                       component_definition);
    }

    auto component_instance_ids_ = parser.FindNodesOfKind("ComponentInstance");
    auto definition_map = detail::GetDefinitionMap(parser);

    for(const auto& component_instance_id : component_instance_ids_) {
        auto definition_uuid = GetUuidFromMedeaId(definition_map.at(component_instance_id));

        ComponentInstance component_instance{parser, component_instance_id, definition_uuid};
        RegisterMedeaId(component_instance.GetCoreData().GetUuid(), component_instance_id);
        component_instances_.emplace(component_instance.GetCoreData().GetUuid(),
                                     component_instance);
    }
}

void ExperimentDefinition::PopulateMiddlewarePorts(GraphmlParser& parser)
{
    auto all_port_definition_ids = parser.FindNodesOfKinds(
        {"PublisherPort", "SubscriberPort", "RequesterPort", "ReplierPort"});

    for(const auto& port_definition_id : all_port_definition_ids) {
        MiddlewarePortDefinition port_definition{parser, port_definition_id};

        // Add port definition to parent
        auto parent_component_definition_id = parser.GetParentNode(port_definition_id);
        component_definitions_.at(GetUuidFromMedeaId(parent_component_definition_id))
            .AddMiddlewarePortDefinition(port_definition);

        RegisterMedeaId(port_definition.GetCoreData().GetUuid(), port_definition_id);
        middleware_port_definitions_.emplace(port_definition.GetCoreData().GetUuid(),
                                             port_definition);
    }

    auto all_port_instance_ids = parser.FindNodesOfKinds(
        {"PublisherPortInstance", "SubscriberPortInstance", "RequesterPortInstance",
         "ReplierPortInstance"});

    auto definition_map = detail::GetDefinitionMap(parser);
    for(const auto& port_instance_id : all_port_instance_ids) {
        auto definition_uuid = GetUuidFromMedeaId(definition_map.at(port_instance_id));
        MiddlewarePortInstance port_instance{parser, port_instance_id, definition_uuid};

        // Add port instance to parent
        auto parent_component_instance_id = parser.GetParentNode(port_instance_id);
        component_instances_.at(GetUuidFromMedeaId(parent_component_instance_id))
            .AddMiddlewarePortInstance(port_instance);

        RegisterMedeaId(port_instance.GetCoreData().GetUuid(), port_instance_id);
        middleware_port_instances_.emplace(port_instance.GetCoreData().GetUuid(), port_instance);
    }
}

void ExperimentDefinition::PopulateAttributes(GraphmlParser& parser)
{
    auto attribute_definition_ids = parser.FindNodesOfKind("Attribute");

    for(const auto& attribute_definition_id : attribute_definition_ids) {
        AttributeDefinition attribute_definition{parser, attribute_definition_id};
        RegisterMedeaId(attribute_definition.GetCoreData().GetUuid(), attribute_definition_id);
        attribute_definitions_.emplace(attribute_definition.GetCoreData().GetUuid(),
                                       attribute_definition);
    }

    auto attribute_instance_ids = parser.FindNodesOfKind("AttributeInstance");
    auto definition_map = detail::GetDefinitionMap(parser);

    for(const auto& attribute_instance_id : attribute_instance_ids) {
        // Check that this is actually an attribute instance with a value.
        //  The type "Attribute Instance" is used in two places, attributes in the assembly aspect
        //  (good)
        //   and attributes in class intances within component impls (bad). Check that we're only
        //   adding attribute instances in the first case.
        // We can check this by checking that our parent's parent is a component instance (if our
        // parent is a class instance)

        auto parent_id = parser.GetParentNode(attribute_instance_id);
        if(parser.GetDataValue(parent_id, "kind") == "ClassInstance") {
            auto grandparent_id = parser.GetParentNode(parent_id);
            if(parser.GetDataValue(grandparent_id, "kind") == "ComponentInstance") {
                // TODO: populate this attribute instance with the correct definition uuid
                auto attribute_instance = ConstructAttributeInstance(parser, attribute_instance_id,
                                                                     sem::types::Uuid{});
                RegisterMedeaId(attribute_instance->GetCoreData().GetUuid(),
                                attribute_instance->GetCoreData().GetMedeaId());
                attribute_instances_.emplace(attribute_instance->GetCoreData().GetUuid(),
                                             std::move(attribute_instance));
            }
        } else {
            auto definition_uuid = GetUuidFromMedeaId(definition_map.at(attribute_instance_id));
            auto attribute_instance = ConstructAttributeInstance(parser, attribute_instance_id,
                                                                 definition_uuid);
            RegisterMedeaId(attribute_instance->GetCoreData().GetUuid(),
                            attribute_instance->GetCoreData().GetMedeaId());
            attribute_instances_.emplace(attribute_instance->GetCoreData().GetUuid(),
                                         std::move(attribute_instance));
        }
    }
}

void ExperimentDefinition::PopulatePortDelegates(GraphmlParser& parser)
{
    auto port_delegate_ids = parser.FindNodesOfKinds(
        {"SubscriberPortDelegate", "PublisherPortDelegate", "RequesterPortDelegate",
         "ReplierPortDelegate"});

    for(const auto& delegate_id : port_delegate_ids) {
        PortDelegateInstance delegate{parser, delegate_id};
        RegisterMedeaId(delegate.GetCoreData().GetUuid(), delegate_id);
        port_delegates_.emplace(delegate.GetCoreData().GetUuid(), delegate);
    }
}

void ExperimentDefinition::ConnectPortsAndPortDelegates(GraphmlParser& parser)
{
    auto assembly_edges = detail::GetAssemblyEdges(parser);

    for(const auto& [source, target] : assembly_edges) {
        auto source_uuid = GetUuidFromMedeaId(source);
        auto target_uuid = GetUuidFromMedeaId(target);

        // Source is a middleware port
        if(middleware_port_instances_.count(source_uuid) != 0) {
            // target is a middleware port
            if(middleware_port_instances_.count(target_uuid) != 0) {
                middleware_port_instances_.at(source_uuid)
                    .AddConnectedMiddlewarePort(middleware_port_instances_.at(target_uuid));
                middleware_port_instances_.at(target_uuid)
                    .AddConnectedMiddlewarePort(middleware_port_instances_.at(source_uuid));
            }

            // target is a port delegate
            else if(port_delegates_.count(target_uuid) != 0) {
                middleware_port_instances_.at(source_uuid)
                    .AddConnectedPortDelegate(port_delegates_.at(target_uuid));
                port_delegates_.at(target_uuid)
                    .AddConnectedMiddlewarePortInstance(middleware_port_instances_.at(source_uuid));
            }
        }

        // Source is a port delegate
        else if(port_delegates_.count(source_uuid) != 0) {
            // target is a middleware port
            if(middleware_port_instances_.count(target_uuid) != 0) {
                port_delegates_.at(source_uuid)
                    .AddConnectedMiddlewarePortInstance(middleware_port_instances_.at(target_uuid));
                middleware_port_instances_.at(target_uuid)
                    .AddConnectedPortDelegate(port_delegates_.at(source_uuid));
            }

            // target is a port delegate
            else if(port_delegates_.count(target_uuid) != 0) {
                port_delegates_.at(source_uuid)
                    .AddConnectedPortDelegate(port_delegates_.at(target_uuid));
                port_delegates_.at(target_uuid)
                    .AddConnectedPortDelegate(port_delegates_.at(source_uuid));
            }
        }
    }
}

void ExperimentDefinition::PopulatePeriodicPorts(GraphmlParser& parser)
{
    auto periodic_port_ids = parser.FindNodesOfKind("PeriodicPort");

    for(const auto& periodic_port_id : periodic_port_ids) {
        auto frequency_attribute_ids = parser.FindImmediateChildren("Attribute", periodic_port_id);

        auto& frequency_attribute = attribute_definitions_.at(
            GetUuidFromMedeaId(frequency_attribute_ids.front()));

        PeriodicPortDefinition periodic_port_definition{parser, periodic_port_id,
                                                        frequency_attribute};
        RegisterMedeaId(periodic_port_definition.GetCoreData().GetUuid(), periodic_port_id);
        periodic_port_definitions_.emplace(periodic_port_definition.GetCoreData().GetUuid(),
                                           periodic_port_definition);
    }

    auto port_instance_ids = parser.FindNodesOfKind("PeriodicPortInstance");
    for(const auto& periodic_port_id : port_instance_ids) {
        auto frequency_attribute_id =
            parser.FindImmediateChildren("AttributeInstance", periodic_port_id).front();
        const auto& frequency_attribute = attribute_instances_.at(
            GetUuidFromMedeaId(frequency_attribute_id));

        auto definition_id = detail::GetDefinitionMap(parser).at(periodic_port_id);
        auto definition_uuid = GetUuidFromMedeaId(definition_id);

        PeriodicPortInstance periodic_port_instance{parser, periodic_port_id,
                                                    periodic_port_definitions_.at(definition_uuid),
                                                    *frequency_attribute};

        auto component_id = parser.GetParentNode(periodic_port_id);
        auto component_instance_uuid = GetUuidFromMedeaId(component_id);

        component_instances_.at(component_instance_uuid)
            .AddPeriodicPortInstance(periodic_port_instance);

        RegisterMedeaId(periodic_port_instance.GetCoreData().GetUuid(), periodic_port_id);
        periodic_port_instances_.emplace(periodic_port_instance.GetCoreData().GetUuid(),
                                         periodic_port_instance);
    }
}

void ExperimentDefinition::PopulateComponentAssemblies(GraphmlParser& parser)
{
    auto component_assembly_ids = parser.FindNodesOfKind("ComponentAssembly");

    // Construct a representation for each component assembly
    for(const auto& component_assembly_id : component_assembly_ids) {
        ComponentAssembly assembly{parser, component_assembly_id};

        // populate assembly's component instances
        auto assembly_component_instances = parser.FindImmediateChildren("ComponentInstance",
                                                                         component_assembly_id);
        for(const auto& assembly_component_instance_id : assembly_component_instances) {
            auto component_uuid = GetUuidFromMedeaId(assembly_component_instance_id);
            assembly.AddComponentInstance(component_instances_.at(component_uuid));
        }

        // populate assembly's port delegates
        auto assembly_port_delegates =
            parser.FindImmediateChildren({"SubscriberPortDelegate", "PublisherPortDelegate",
                                          "RequesterPortDelegate", "ReplierPortDelegate"},
                                         component_assembly_id);
        for(const auto& port_delegate_id : assembly_port_delegates) {
            auto delegate_uuid = GetUuidFromMedeaId(port_delegate_id);
            assembly.AddPortDelegateInstance(port_delegates_.at(delegate_uuid));
        }

        RegisterMedeaId(assembly.GetCoreData().GetUuid(), component_assembly_id);
        component_assemblies_.emplace(assembly.GetCoreData().GetUuid(), assembly);
    }

    // Go through all our component assemblies and add children to parents.
    for(auto& [uuid, component_assembly] : component_assemblies_) {
        auto child_assemblies = parser.FindImmediateChildren(
            "ComponentAssembly", component_assembly.GetCoreData().GetMedeaId());

        for(const auto& child_assembly_id : child_assemblies) {
            auto child_assembly = component_assemblies_.at(GetUuidFromMedeaId(child_assembly_id));
            component_assembly.AddComponentAssembly(child_assembly);
        }
    }
}

void ExperimentDefinition::PopulateWorkers(GraphmlParser& parser)
{
    auto worker_definition_ids = parser.FindNodesOfKind("Class");
    for(const auto& worker_definition_id : worker_definition_ids) {
        WorkerDefinition worker_definition{parser, worker_definition_id};

        auto attribute_ids = parser.FindImmediateChildren("Attribute", worker_definition_id);
        for(const auto& attribute_id : attribute_ids) {
            auto uuid = GetUuidFromMedeaId(attribute_id);
            worker_definition.AddAttributeDefinition(attribute_definitions_.at(uuid));
        }
        RegisterMedeaId(worker_definition.GetCoreData().GetUuid(), worker_definition_id);
        worker_definitions_.emplace(worker_definition.GetCoreData().GetUuid(), worker_definition);
    }

    auto worker_instance_ids = parser.FindNodesOfKind("ClassInstance");
    for(const auto& worker_instance_id : worker_instance_ids) {
        // Check that our parent is a component instance, skip if otherwise.
        //  "ClassInstance" is overloaded in medea to include ClassInstances in both the Assembly
        //  and Behaviour aspects. We don't care about ClassInterfaces in behaviour at the moment.
        auto parent_id = parser.GetParentNode(worker_instance_id);
        if(parser.GetDataValue(parent_id, "kind") != "ComponentInstance") {
            continue;
        }

        // Go through two hops of definition to get our actual definition. This is needed due to the
        //  aforementioned overloading of "ClassInstance"
        auto def_map = detail::GetDefinitionMap(parser);
        auto behaviour_worker_instance_id = def_map.at(worker_instance_id);
        auto worker_definition_id = def_map.at(behaviour_worker_instance_id);

        auto definition_uuid = GetUuidFromMedeaId(worker_definition_id);
        WorkerInstance worker_instance{parser, worker_instance_id,
                                       worker_definitions_.at(definition_uuid)};

        auto attribute_inst_ids = parser.FindImmediateChildren("AttributeInstance",
                                                               worker_instance_id);

        for(const auto& attribute_instance_id : attribute_inst_ids) {
            auto attribute_instance_uuid = GetUuidFromMedeaId(attribute_instance_id);
            worker_instance.AddAttributeInstance(*attribute_instances_.at(attribute_instance_uuid));
        }

        worker_instances_.emplace(worker_instance.GetCoreData().GetUuid(), worker_instance);
    }
}

void ExperimentDefinition::DeployComponentInstances(GraphmlParser& parser)
{
    auto deployment_map = detail::GetDeploymentMap(parser);
    for(const auto& [component_uuid, component_instance] : component_instances_) {
        auto instance_medea_id = component_instance.GetCoreData().GetMedeaId();

        // Check that this component instance is directly deployed
        if(deployment_map.count(instance_medea_id)) {
            auto deployment_location_medea_id = deployment_map.at(instance_medea_id).front();
            auto deployment_location_uuid = GetUuidFromMedeaId(deployment_location_medea_id);

            // Deploy it to the right place
            if(container_definitions_.count(deployment_location_uuid)) {
                container_definitions_.at(deployment_location_uuid)
                    .AddDeployedComponentInstance(component_instance);
            } else if(node_definitions_.count(deployment_location_uuid)) {
                node_definitions_.at(deployment_location_uuid)
                    .AddDeployedComponentInstance(component_instance);
            } else if(cluster_definitions_.count(deployment_location_uuid)) {
                cluster_definitions_.at(deployment_location_uuid)
                    .AddDeployedComponentInstance(component_instance);
            }
        }
    }
}

void ExperimentDefinition::DeployComponentAssemblies(GraphmlParser& parser)
{
    auto deployment_map = detail::GetDeploymentMap(parser);
    for(const auto& [component_uuid, component_assembly] : component_assemblies_) {
        auto assembly_medea_id = component_assembly.GetCoreData().GetMedeaId();

        // Check that this component instance is directly deployed
        if(deployment_map.count(assembly_medea_id)) {
            auto deployment_location_medea_id = deployment_map.at(assembly_medea_id).front();
            auto deployment_location_uuid = GetUuidFromMedeaId(deployment_location_medea_id);

            // Deploy it to the right place
            if(container_definitions_.count(deployment_location_uuid)) {
                container_definitions_.at(deployment_location_uuid)
                    .AddDeployedComponentAssembly(component_assembly);
            } else if(node_definitions_.count(deployment_location_uuid)) {
                node_definitions_.at(deployment_location_uuid)
                    .AddDeployedComponentAssembly(component_assembly);
            } else if(cluster_definitions_.count(deployment_location_uuid)) {
                cluster_definitions_.at(deployment_location_uuid)
                    .AddDeployedComponentAssembly(component_assembly);
            }
        }
    }
}
auto ExperimentDefinition::SetName(const std::string& experiment_name) -> void
{
    experiment_name_ = experiment_name;
}
auto ExperimentDefinition::GetUuid() -> sem::types::Uuid
{
    return uuid_;
}

auto ExperimentDefinition::GetContainerToStartOnTriggerEvent(const std::string& trigger_medea_id)
    -> Container
{
    for(const auto& [uuid, trigger] : trigger_instances_) {
        if(trigger.GetCoreData().GetMedeaId() == trigger_medea_id) {
            auto strategy_uuid = trigger.GetStrategyUuid();
            auto container_uuid = strategies_.at(strategy_uuid).GetContainerUuid();
            return container_definitions_.at(container_uuid);
        }
    }
}
auto ExperimentDefinition::GetTriggerInstFromMedeaId(const std::string& trigger_inst_id) const
    -> TriggerInstance
{
    for(const auto& [uuid, trigger_inst] : trigger_instances_) {
        if(trigger_inst.GetCoreData().GetMedeaId() == trigger_inst_id) {
            return trigger_inst;
        }
    }
}

auto ExperimentDefinition::GetTriggerDefinition(const TriggerInstance& instance) const
    -> TriggerDefinition
{
    return trigger_definitions_.at(instance.GetDefinitionUuid());
}

auto ExperimentDefinition::GetDeploymentLocation(const Container& container) const
    -> std::variant<Node, Cluster>
{
    for(const auto& [uuid, cluster] : cluster_definitions_) {
        if(cluster.HasDeployedContainer(container)) {
            return cluster;
        }
    }

    for(const auto& [uuid, node] : node_definitions_) {
        if(node.HasDeployedContainer(container)) {
            return node;
        }
    }
    throw std::out_of_range("Container not deployed!");
}
auto ExperimentDefinition::GetContainersComponents(const Container& container) const
    -> std::vector<ComponentInstance>
{
    std::vector<ComponentInstance> out;
    for(const auto& component_uuid : container.GetDeployedComponents()) {
        out.push_back(component_instances_.at(component_uuid));
    }
    return out;
}
auto ExperimentDefinition::GetComponentInstanceDefinition(
    const ComponentInstance& component_instance) const -> ComponentDefinition
{
    auto def_uuid = component_instance.GetDefinitionUuid();
    return component_definitions_.at(def_uuid);
}
void ExperimentDefinition::PopulateTriggers(GraphmlParser& parser)
{
    auto trigger_definition_ids = parser.FindNodesOfKind("Trigger");

    for(const auto& trigger_def_id : trigger_definition_ids) {
        TriggerDefinition trigger_definition{parser, trigger_def_id};
        RegisterMedeaId(trigger_definition.GetCoreData().GetUuid(),
                        trigger_definition.GetCoreData().GetMedeaId());
        trigger_definitions_.emplace(trigger_definition.GetCoreData().GetUuid(),
                                     trigger_definition);
    }

    auto trigger_instance_ids = parser.FindNodesOfKind("TriggerInstance");

    auto definition_map = detail::GetDefinitionMap(parser);

    for(const auto& trigger_inst_id : trigger_instance_ids) {
        auto definition_id = definition_map.at(trigger_inst_id);
        auto definition_uuid = GetUuidFromMedeaId(definition_id);

        TriggerInstance trigger_instance{parser, trigger_inst_id,
                                         trigger_definitions_.at(definition_uuid)};

        RegisterMedeaId(trigger_instance.GetCoreData().GetUuid(),
                        trigger_instance.GetCoreData().GetMedeaId());
        auto parent_component_instance_id = parser.GetParentNode(trigger_inst_id);
        auto parent_uuid = GetUuidFromMedeaId(parent_component_instance_id);
        component_instances_.at(parent_uuid).AddTriggerInstance(trigger_instance);

        trigger_instances_.emplace(trigger_instance.GetCoreData().GetUuid(), trigger_instance);
    }
}
void ExperimentDefinition::PopulateStrategies(GraphmlParser& parser)
{
    auto strat_instance_ids = parser.FindNodesOfKind("StrategyInstance");

    for(const auto& strategy_instance_id : strat_instance_ids) {
        auto container_id = parser.GetDataValue(strategy_instance_id, "Container_reference");
        auto container_uuid = GetUuidFromMedeaId(container_id);
        Strategy strategy{parser, strategy_instance_id, container_definitions_.at(container_uuid)};
        RegisterMedeaId(strategy.GetCoreData().GetUuid(), strategy.GetCoreData().GetMedeaId());

        auto parent_id = parser.GetParentNode(strategy_instance_id);
        auto parent_uuid = GetUuidFromMedeaId(parent_id);

        trigger_instances_.at(parent_uuid).AddStrategy(strategy);

        strategies_.emplace(strategy.GetCoreData().GetUuid(), strategy);
    }
}
auto ExperimentDefinition::GetMiddlewarePortDefinition(const MiddlewarePortInstance& instance) const
    -> MiddlewarePortDefinition
{
    auto definition_uuid = instance.GetDefinitionUuid();
    return middleware_port_definitions_.at(definition_uuid);
}
auto ExperimentDefinition::GetComponentInstanceMiddlewarePorts(
    const ComponentInstance& component_instance) const -> std::vector<MiddlewarePortInstance>
{
    std::vector<MiddlewarePortInstance> out;
    for(const auto& port_inst_uuid : component_instance.GetMiddlewarePortInstanceUuids()) {
        out.push_back(middleware_port_instances_.at(port_inst_uuid));
    }
    return out;
}
auto ExperimentDefinition::GetComponentInstancePeriodicPorts(
    const ComponentInstance& component_instance) const -> std::vector<PeriodicPortInstance>
{
    std::vector<PeriodicPortInstance> out;
    for(const auto& port_inst_uuid : component_instance.GetPeriodicPortInstanceUuids()) {
        out.push_back(periodic_port_instances_.at(port_inst_uuid));
    }
    return out;
}
auto ExperimentDefinition::GetPeriodicPortFrequencyAttribute(
    const PeriodicPortInstance& periodic_port_instance) const -> double
{
    auto attribute_uuid = periodic_port_instance.GetFrequencyAttributeUuid();

    auto& interface_ref = attribute_instances_.at(attribute_uuid);
    // TODO: fix that we serialise to proto to get our attribute out. yikes.
    double out = interface_ref->ToProto()->d();
}
auto ExperimentDefinition::GetWorkerDefinition(const WorkerInstance& inst) const -> WorkerDefinition
{
    return worker_definitions_.at(inst.GetDefinitionUuid());
}
auto ExperimentDefinition::GetComponentInstanceWorkers(const ComponentInstance& instance) const
    -> std::vector<WorkerInstance>
{
    std::vector<WorkerInstance> out;
    for(const auto& worker_inst_uuid : instance.GetWorkerInstanceUuids()) {
        out.push_back(worker_instances_.at(worker_inst_uuid));
    }
    return out;
}
auto ExperimentDefinition::GetComponentAttributeInstances(const ComponentInstance& component) const
    -> std::vector<std::unique_ptr<AttributeInstancePb>>
{
    std::vector<std::unique_ptr<AttributeInstancePb>> out;
    for(const auto& attribute_uuid : component.GetAttributeInstanceUuids()) {
        out.emplace_back(attribute_instances_.at(attribute_uuid)->ToProto());
    }
    return out;
}

} // namespace re::Representation
