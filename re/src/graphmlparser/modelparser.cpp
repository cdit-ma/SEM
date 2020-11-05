#include "modelparser.h"
#include <algorithm>
#include <fstream>
#include <google/protobuf/util/json_util.h>
#include <iostream>
#include <memory>
#include "helper.h"
#include <sstream>
#include <string>

#include "graphmlparser.h"
namespace re::ModelParser {

std::unique_ptr<NodeManager::Experiment>
ModelParser::ParseModel(const std::string& filename, const std::string& experiment_id)
{
    std::ifstream model_stream(filename);
    ModelParser parser(model_stream, experiment_id);
    return std::make_unique<NodeManager::Experiment>(parser.GetExperiment());
}

std::unique_ptr<NodeManager::Experiment>
ModelParser::ParseModelString(const std::string& model_string, const std::string& experiment_id)
{
    std::istringstream model_stream{model_string};
    ModelParser parser(model_stream, experiment_id);
    return std::make_unique<NodeManager::Experiment>(parser.GetExperiment());
}

// Pretty json prints deployment protobuf experiment message
std::string ModelParser::GetDeploymentJSON(const NodeManager::Experiment& experiment)
{
    std::string output;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = false;

    google::protobuf::util::MessageToJsonString(experiment, &output, options);
    return output;
}

ModelParser::ModelParser(std::istream& model_stream, const std::string& experiment_id)
{
    experiment_id_ = experiment_id;
    graphml_parser_ = std::make_unique<GraphmlParser>(model_stream);
    PreProcess();
    Process();
}

// Starting at a node, recurse through all edges originating from it (of a particular edge kind).
// Returns all nodes which don't have edges (of a particular edge kind) originating from them.
/* Visual Representation

    [B]-------|
                V
    [C]----->[A]---->[X]
                ʌ
                |
    [E]----->[D]
                ʌ
                |
                [F]

    Calling this function on node X would return {B,C,E,F} but ignore {A,D}
*/
std::set<std::string> ModelParser::GetTerminalSourcesByEdgeKind(const std::string& node_id,
                                                                const std::string& edge_kind,
                                                                std::set<std::string> prev_ids)
{
    std::set<std::string> source_ids;

    bool is_assembly_edge = edge_kind == "Edge_Assembly";

    // Detect cycle
    if(prev_ids.count(node_id)) {
        throw std::runtime_error("Cyclic link exists in edges " + edge_kind
                                 + " containing node id: " + node_id);
    }
    prev_ids.insert(node_id);

    // Get all edges connected to node_id
    for(const auto& edge_id : entity_edge_ids_[node_id]) {
        if(edge_kind == graphml_parser_->GetDataValue(edge_id, "kind")) {
            // If node provided is the target of this edge
            if(node_id == graphml_parser_->GetAttribute(edge_id, "target")) {
                // Get the source of this edge
                const auto& edge_source = graphml_parser_->GetAttribute(edge_id, "source");

                bool recurse_edges = true;
                if(is_assembly_edge) {
                    // Treat External Delegate Ports as Terminations so we don't break intra-model
                    // connectivity
                    const auto& source_kind = graphml_parser_->GetDataValue(edge_source, "kind");
                    if(source_kind == "PubSubPortDelegate"
                       || source_kind == "RequestPortDelegate") {
                        recurse_edges = false;
                    }
                }

                // Get the edges originating from the source
                const auto& connected_sources = GetTerminalSourcesByEdgeKind(edge_source, edge_kind,
                                                                             prev_ids);

                // If we have edges originating from the source, append them
                if(recurse_edges && !connected_sources.empty()) {
                    for(const auto& source_id : connected_sources) {
                        source_ids.insert(source_id);
                    }
                } else {
                    // If this node has no other edges, it itself is a terminal endpoint
                    // So add it to our source_ids set
                    source_ids.insert(edge_source);
                }
            }
        }
    }

    return source_ids;
}

// Get replication count of entity with supplied id
unsigned int ModelParser::GetReplicationId(const std::string& id)
{
    unsigned int replication_count = 1;
    try {
        // Try get a variable child
        const auto& variable_children = graphml_parser_->FindImmediateChildren("Variable", id);
        // Get the first Variable child of the Component Assembly, and get its recursed data value
        // as an uint
        for(const auto& variable_id : variable_children) {
            const auto& attribute_val = attribute_value_map_[variable_id];
            auto replicate_value = std::stoi(attribute_val);
            if(replicate_value < 0) {
                std::cerr << "GOT: " << attribute_val << std::endl;
                replicate_value = 0;
            }
            replication_count = replicate_value;
            break;
        }
    } catch(const std::exception& ex) {
    }
    return replication_count;
}

// Generate internal representation of replications of supplied Assembly and its children (both
// assemblies and instances). Should only be called with assembly definition aspect top level, all
// other calls are handled internally.
void ModelParser::GenerateReplications(Assembly& current_assembly)
{
    const auto& assembly_id = current_assembly.GetAssemblyId();
    const auto& replication_count = GetReplicationId(assembly_id);
    current_assembly.SetReplicationCount(replication_count);

    if(replication_count > 0) {
        // Get the list of children
        auto assembly_children = graphml_parser_->FindImmediateChildren("ComponentAssembly",
                                                                        assembly_id);
        auto component_children = graphml_parser_->FindImmediateChildren("ComponentInstance",
                                                                         assembly_id);

        // Populate initial case and its children
        current_assembly.SetReplicationIndex(0);
        GenerateChildAssemblies(current_assembly, assembly_children);
        GenerateComponentReplications(current_assembly, component_children);

        // Populate replications of current_assembly and their children
        for(unsigned int i = 1; i < current_assembly.GetReplicationCount(); ++i) {
            auto replicated_assembly = std::make_unique<Assembly>(current_assembly);

            replicated_assembly->SetReplicationIndex(i);
            GenerateChildAssemblies(*replicated_assembly, assembly_children);
            GenerateComponentReplications(*replicated_assembly, component_children);

            // We have to do this check to avoid the top level case.
            if(current_assembly.GetParent()) {
                current_assembly.GetParent()->AddChild(std::move(replicated_assembly));
            }
        }
    }
}

void ModelParser::GenerateChildAssemblies(Assembly& assembly,
                                          const std::vector<std::string>& assembly_children)
{
    for(const auto& child_assembly_id : assembly_children) {
        auto child_assembly =
            std::make_unique<Assembly>(graphml_parser_->GetDataValue(child_assembly_id, "label"),
                                       child_assembly_id, &assembly);

        GenerateReplications(*child_assembly);
        assembly.AddChild(std::move(child_assembly));
    }
}

void ModelParser::GenerateComponentReplications(Assembly& assembly,
                                                const std::vector<std::string>& component_children)
{
    for(const auto& component_child_id : component_children) {
        auto component = std::make_unique<ComponentReplication>(component_child_id, &assembly);

        // XXX: component_instances_ pushed back onto this vector can have their parent* invalidated
        // if the top level node is deleted before component_instances_
        component_instances_.emplace_back(std::move(component));
    }
}

void ModelParser::PreProcess()
{
    // Pre-condition check
    assert(graphml_parser_);

    auto assembly_definitions = graphml_parser_->FindNodesOfKind("AssemblyDefinitions");

    if(assembly_definitions.size() != 1) {
        std::cerr << "Don't have an Assembly Definition! Cannot continue" << std::endl;
        return;
    }

    auto assembly_definition_id = assembly_definitions.front();

    // Get all ids
    hardware_node_ids_ = graphml_parser_->FindNodesOfKind("HardwareNode");
    hardware_cluster_ids_ = graphml_parser_->FindNodesOfKind("HardwareCluster");
    deployment_container_ids_ = graphml_parser_->FindNodesOfKind("DeploymentContainer");

    logging_server_ids_ = graphml_parser_->FindNodesOfKind("LoggingServer");
    logging_client_ids_ = graphml_parser_->FindNodesOfKind("LoggingProfile");

    delegates_pubsub_ids_ = graphml_parser_->FindNodesOfKind("PubSubPortDelegate");
    delegates_server_ids_ = graphml_parser_->FindNodesOfKind("RequestPortDelegate");

    // Get the ID's of the edges
    deployment_edge_ids_ = graphml_parser_->FindEdges("Edge_Deployment");
    assembly_edge_ids_ = graphml_parser_->FindEdges("Edge_Assembly");
    definition_edge_ids_ = graphml_parser_->FindEdges("Edge_Definition");
    aggregate_edge_ids_ = graphml_parser_->FindEdges("Edge_Aggregate");
    qos_edge_ids_ = graphml_parser_->FindEdges("Edge_QOS");

    for(const auto& edge_id : graphml_parser_->FindEdges()) {
        auto source_id = graphml_parser_->GetAttribute(edge_id, "source");
        auto target_id = graphml_parser_->GetAttribute(edge_id, "target");
        entity_edge_ids_[source_id].insert(edge_id);
        entity_edge_ids_[target_id].insert(edge_id);
    }

    // Construct a Deployment Map which points ComponentInstance - > HardwareNodes
    for(const auto& e_id : deployment_edge_ids_) {
        auto source_id = graphml_parser_->GetAttribute(e_id, "source");
        auto target_id = graphml_parser_->GetAttribute(e_id, "target");
        deployed_entities_map_[source_id].emplace_back(target_id);
    }

    // Find the values for the AttributesInstances/Variables in Assembly which has been data linked
    for(const auto& attribute_id : graphml_parser_->FindNodesOfKinds({"AttributeInstance", "Variabl"
                                                                                           "e"},
                                                                     assembly_definition_id)) {
        // Get all ids which feed attribute_id with Data
        auto data_sources = GetTerminalSourcesByEdgeKind(attribute_id, "Edge_Data", {});

        auto data_value = graphml_parser_->GetDataValue(attribute_id, "value");
        if(data_sources.size() == 1) {
            data_value = graphml_parser_->GetDataValue(*(data_sources.begin()), "value");
        }
        attribute_value_map_[attribute_id] = data_value;
    }

    top_level_assembly_ = std::make_unique<Assembly>(
        graphml_parser_->GetDataValue(assembly_definition_id, "label"), assembly_definition_id);

    GenerateReplications(*top_level_assembly_);

    // Construct a Deployment Map which points EventPort - > QOSProfile
    for(const auto& e_id : qos_edge_ids_) {
        auto source_id = graphml_parser_->GetAttribute(e_id, "source");
        auto target_id = graphml_parser_->GetAttribute(e_id, "target");
        entity_qos_map_[source_id] = target_id;
    }

    std::vector<std::string> endpoint_ids;

    auto sub_port_ids = graphml_parser_->FindNodesOfKind("SubscriberPortInstance");
    auto rep_port_ids = graphml_parser_->FindNodesOfKind("ReplierPortInstance");

    endpoint_ids.insert(endpoint_ids.end(), sub_port_ids.begin(), sub_port_ids.end());
    endpoint_ids.insert(endpoint_ids.end(), rep_port_ids.begin(), rep_port_ids.end());
    endpoint_ids.insert(endpoint_ids.end(), delegates_pubsub_ids_.begin(),
                        delegates_pubsub_ids_.end());
    endpoint_ids.insert(endpoint_ids.end(), delegates_server_ids_.begin(),
                        delegates_server_ids_.end());

    // Populate port connection map using recurse edge function to follow port delegates through
    for(const auto& port_id : endpoint_ids) {
        const auto& target_id = port_id;
        for(const auto& source_id : GetTerminalSourcesByEdgeKind(target_id, "Edge_Assembly", {})) {
            AssemblyConnection edge;
            edge.source_id = source_id;
            edge.target_id = target_id;

            // Check to see if our Ports parents are the same
            auto src_pid = graphml_parser_->GetParentNode(source_id, 2);
            auto dst_pid = graphml_parser_->GetParentNode(target_id, 2);
            // Component->EventPort, so checking two parents up should tell us if we are contained
            // within the same Assembly
            edge.inter_assembly = src_pid != dst_pid;
            assembly_map_[source_id].push_back(edge);
        }
    }

    for(const auto& edge_id : assembly_edge_ids_) {
        auto target_id = graphml_parser_->GetAttribute(edge_id, "target");
        auto source_id = graphml_parser_->GetAttribute(edge_id, "source");
        if(std::find(logging_server_ids_.begin(), logging_server_ids_.end(), target_id)
           != logging_server_ids_.end()) {
            logging_server_client_map_[target_id].push_back(source_id);
        }
    }
    for(const auto& edge_id : graphml_parser_->FindEdges("Edge_Trigger")) {
        auto target_id = graphml_parser_->GetAttribute(edge_id, "target");
        auto source_id = graphml_parser_->GetAttribute(edge_id, "source");
        trigger_edge_ids_[source_id] = target_id;
    }
}

void ModelParser::ParseHardwareItems()
{
    // Adds implicit containers to all hardware items. Components are distributed evenly to implicit
    // containers when deployed to a cluster.
    for(const auto& cluster_id : hardware_cluster_ids_) {
        auto cluster = experiment_->add_clusters();
        cluster->mutable_info()->set_id(cluster_id);
        cluster->mutable_info()->set_name(graphml_parser_->GetDataValue(cluster_id, "label"));

        // Generate implicit container for the cluster
        auto implicit_container = cluster->add_containers();
        implicit_container->set_implicit(true);
        implicit_container->mutable_info()->set_id(cluster_id + "_0");
        implicit_container->mutable_info()->set_name(
            graphml_parser_->GetDataValue(cluster_id, "label"));
        implicit_container->set_type(NodeManager::Container::GENERIC);

        container_message_map_[cluster_id] = implicit_container;
        cluster_message_map_[cluster_id] = cluster;
    }

    for(const auto& hardware_id : hardware_node_ids_) {
        auto parent_id = graphml_parser_->GetParentNode(hardware_id);

        // Check that current hardware node belongs to a cluster and is online
        // i.e. Ignore localhost node and offline nodes
        // TODO: Re-introduce deployment to localhost node.
        bool is_offline = graphml_parser_->GetDataValue(hardware_id, "ip_address") == "OFFLINE";
        bool has_parent_cluster = cluster_message_map_.count(parent_id) > 0;

        if(!is_offline && has_parent_cluster) {
            auto node = cluster_message_map_[parent_id]->add_nodes();

            node->mutable_info()->set_id(hardware_id);
            node->mutable_info()->set_name(graphml_parser_->GetDataValue(hardware_id, "label"));
            node->set_ip_address(graphml_parser_->GetDataValue(hardware_id, "ip_address"));

            // Generate implicit container for the hardware node
            auto implicit_container = node->add_containers();
            implicit_container->set_implicit(true);
            implicit_container->mutable_info()->set_id(hardware_id + "_0");
            implicit_container->mutable_info()->set_name(
                graphml_parser_->GetDataValue(hardware_id, "label"));
            implicit_container->set_type(NodeManager::Container::GENERIC);

            container_message_map_[hardware_id] = implicit_container;
            node_message_map_[hardware_id] = node;
        }
    }

    for(const auto& deployment_container_id : deployment_container_ids_) {
        auto deployed_id = GetDeployedID(deployment_container_id);
        NodeManager::Container* container = nullptr;

        if(cluster_message_map_.count(deployed_id)) {
            // If we're deployed to a cluster, add a container to the cluster, otherwise add
            // container to node
            container = cluster_message_map_[deployed_id]->add_containers();
        } else if(node_message_map_.count(deployed_id)) {
            container = node_message_map_[deployed_id]->add_containers();
        }

        if(container) {
            container->mutable_info()->set_id(deployment_container_id);
            container->mutable_info()->set_name(
                graphml_parser_->GetDataValue(deployment_container_id, "label"));
            if(graphml_parser_->GetDataValue(deployment_container_id, "is_docker") == "true") {
                container->set_type(NodeManager::Container::DOCKER);
            } else {
                container->set_type(NodeManager::Container::GENERIC);
            }

            container->set_is_late_joiner(
                graphml_parser_->GetDataValue(deployment_container_id, "is_late_joiner") == "true");
            container_message_map_[deployment_container_id] = container;
        }
    }
}

void ModelParser::ParseExternalDelegates()
{
    std::vector<std::string> delegate_ids;
    delegate_ids.insert(delegate_ids.end(), delegates_pubsub_ids_.begin(),
                        delegates_pubsub_ids_.end());
    delegate_ids.insert(delegate_ids.end(), delegates_server_ids_.begin(),
                        delegates_server_ids_.end());

    for(const auto& port_id : delegate_ids) {
        auto parent_id = graphml_parser_->GetParentNode(port_id);
        auto eport_pb = experiment_->add_external_ports();
        auto eport_info_pb = eport_pb->mutable_info();
        eport_info_pb->set_id(port_id);
        auto label = graphml_parser_->GetDataValue(parent_id, "label");
        eport_info_pb->set_name(label);
        eport_info_pb->set_type(graphml_parser_->GetDataValue(port_id, "type"));

        auto port_kind = GetExternalPortKind(graphml_parser_->GetDataValue(port_id, "kind"));
        eport_pb->set_kind(port_kind);

        bool is_blackbox = graphml_parser_->GetDataValue(parent_id, "blackbox") == "true";
        eport_pb->set_is_blackbox(is_blackbox);

        // Set Middleware
        auto middleware_str = graphml_parser_->GetDataValue(port_id, "middleware");
        auto middleware = NodeManager::NO_MIDDLEWARE;

        if(!NodeManager::Middleware_Parse(middleware_str, &middleware)) {
            std::cerr << "Cannot parse middleware: " << middleware_str << std::endl;
        }
        eport_pb->set_middleware(middleware);

        auto attrs = eport_pb->mutable_attributes();

        if(is_blackbox) {
            switch(middleware) {
                case NodeManager::QPID: {
                    // QPID Requires Topic and Broker Addresss
                    const auto& topic_name = graphml_parser_->GetDataValue(port_id, "topic_name");
                    const auto& broker_addr = graphml_parser_->GetDataValue(port_id, "qpid_broker_"
                                                                                     "address");

                    NodeManager::SetStringAttribute(attrs, "topic_name", topic_name);
                    NodeManager::SetStringAttribute(attrs, "broker_address", broker_addr);
                    break;
                }
                case NodeManager::ZMQ: {
                    // ZMQ Requires zmq_publisher_address (PUBSUB) or zmq_server_address (SERVER)
                    if(port_kind == NodeManager::ExternalPort::PUBSUB) {
                        const auto& pub_addr = graphml_parser_->GetDataValue(port_id, "zmq_"
                                                                                      "publisher_"
                                                                                      "address");
                        NodeManager::SetStringAttribute(attrs, "publisher_address", pub_addr);
                    } else if(port_kind == NodeManager::ExternalPort::SERVER) {
                        auto serv_addr = graphml_parser_->GetDataValue(port_id, "zmq_server_"
                                                                                "address");
                        NodeManager::SetStringAttribute(attrs, "server_address", serv_addr);
                    }
                    break;
                }
                case NodeManager::RTI:
                case NodeManager::OSPL: {
                    // DDS Requires domain_id and topic
                    const auto& topic_name = graphml_parser_->GetDataValue(port_id, "topic_name");
                    const auto& domain_id = std::stoi(graphml_parser_->GetDataValue(port_id, "dds_"
                                                                                             "domai"
                                                                                             "n_"
                                                                                             "id"));

                    NodeManager::SetStringAttribute(attrs, "topic_name", topic_name);
                    NodeManager::SetIntegerAttribute(attrs, "domain_id", domain_id);
                    break;
                }
                case NodeManager::TAO: {
                    // TAO Requires tao_orb_endpoint
                    const auto& naming_server_endpoint = graphml_parser_->GetDataValue(port_id,
                                                                                       "tao_naming_"
                                                                                       "service_"
                                                                                       "endpoint");
                    auto server_name_list = graphml_parser_->GetDataValue(port_id, "tao_server_"
                                                                                   "name");
                    auto server_kind = graphml_parser_->GetDataValue(port_id, "tao_server_kind");

                    // Split slash seperated server name into seperate strings.
                    std::transform(server_name_list.begin(), server_name_list.end(),
                                   server_name_list.begin(),
                                   [](char ch) { return ch == '/' ? ' ' : ch; });
                    std::istringstream iss(server_name_list);
                    std::vector<std::string> split_server_name(
                        std::istream_iterator<std::string>{iss},
                        std::istream_iterator<std::string>());

                    NodeManager::SetStringAttribute(attrs, "naming_server_endpoint",
                                                    naming_server_endpoint);
                    NodeManager::SetStringListAttribute(attrs, "server_name", split_server_name);
                    NodeManager::SetStringAttribute(attrs, "server_kind", server_kind);
                    break;
                }
                default:
                    break;
            }
        }

        if(!external_port_id_map_.count(port_id)) {
            external_port_id_map_[port_id] = eport_pb;
        }
    }
}

std::string ModelParser::GetDeployedContainerID(const std::string& component_id)
{
    auto deployed_id = GetDeployedID(component_id);

    if(deployed_id.empty()) {
        auto parent_id = graphml_parser_->GetParentNode(component_id);
        if(!parent_id.empty()) {
            return GetDeployedContainerID(parent_id);
        }
    }
    return deployed_id;
}

std::string ModelParser::GetDeployedID(const std::string& id)
{
    if(deployed_entities_map_.count(id)) {
        auto& ids = deployed_entities_map_[id];
        if(!ids.empty()) {
            return ids.front();
        }
    }
    return std::string("");
}

void ModelParser::ParseLoggingClients()
{
    for(const auto& client_id : logging_client_ids_) {
        // Get pb message that this logger is deployed to

        for(const auto& deployed_id : deployed_entities_map_[client_id]) {
            NodeManager::Logger* logger_pb = nullptr;

            // Check our message maps for the entity that we're deployed to
            if(container_message_map_.count(deployed_id)) {
                logger_pb = container_message_map_.at(deployed_id)->add_loggers();
            }
            if(!logger_pb) {
                continue;
            }

            logger_pb->set_type(NodeManager::Logger::CLIENT);
            logger_pb->set_id(client_id);
            logger_pb->set_frequency(std::stod(graphml_parser_->GetDataValue(client_id, "frequenc"
                                                                                        "y")));

            auto processes = graphml_parser_->GetDataValue(client_id, "processes_to_log");
            std::transform(processes.begin(), processes.end(), processes.begin(),
                           [](char ch) { return ch == ',' ? ' ' : ch; });

            std::istringstream iss(processes);
            std::vector<std::string> split_processes(std::istream_iterator<std::string>{iss},
                                                     std::istream_iterator<std::string>());

            for(const auto& process : split_processes) {
                logger_pb->add_processes(process);
            }

            if(graphml_parser_->GetDataValue(client_id, "mode") == "LIVE") {
                logger_pb->set_mode(NodeManager::Logger::LIVE);
            } else if(graphml_parser_->GetDataValue(client_id, "mode") == "CACHED") {
                logger_pb->set_mode(NodeManager::Logger::CACHED);
            } else if(graphml_parser_->GetDataValue(client_id, "mode") == "OFF") {
                logger_pb->set_mode(NodeManager::Logger::OFF);
            }
        }
    }
}

void ModelParser::ParseLoggingServers()
{
    for(const auto& server_id : logging_server_ids_) {
        // Get pb message that this logger is deployed to
        auto deployed_id = GetDeployedID(server_id);
        NodeManager::Logger* logger_pb = nullptr;

        // Check our message maps for the entity that we're deployed to
        if(container_message_map_.count(deployed_id)) {
            logger_pb = container_message_map_.at(deployed_id)->add_loggers();
        }
        if(!logger_pb) {
            continue;
        }

        logger_pb->set_type(NodeManager::Logger::SERVER);
        logger_pb->set_id(server_id);
        logger_pb->set_db_file_name(graphml_parser_->GetDataValue(server_id, "database"));

        // get connected log client ids
        if(logging_server_client_map_.count(server_id)) {
            for(const auto& client_id : logging_server_client_map_.at(server_id)) {
                logger_pb->add_client_ids(client_id);
            }
        }
    }
}

void ModelParser::ParseComponents()
{
    for(auto& component_instance : component_instances_) {
        const auto& component_id = component_instance->component_instance_id_;
        const auto& component_def_id = GetDefinitionId(component_id);
        const auto& component_uid = component_instance->GetUniqueId();
        const auto& unique_suffix = component_instance->GetUniqueIdSuffix();

        // Get id of deployed node

        auto deployed_id = GetDeployedContainerID(component_id);

        if(deployed_id.empty()) {
            continue;
        }

        NodeManager::Component* component_pb = nullptr;
        if(container_message_map_.count(deployed_id)) {
            component_pb = container_message_map_.at(deployed_id)->add_components();
        }

        if(!component_pb) {
            // Something went wrong, continue
            continue;
        }

        // Add the Component to the replication map
        component_replications_[component_id].push_back(component_pb);
        auto component_info_pb = component_pb->mutable_info();
        component_info_pb->set_id(component_uid);
        component_info_pb->set_name(graphml_parser_->GetDataValue(component_id, "label"));
        component_info_pb->set_type(graphml_parser_->GetDataValue(component_def_id, "label"));

        for(const auto& ns : GetNamespaceList(component_def_id)) {
            component_info_pb->add_namespaces(ns);
        }

        const auto& replication_indices = component_instance->GetReplicationIndices();
        const auto& location = component_instance->GetReplicationLocation();

        *(component_pb->mutable_replicate_indices()) = {replication_indices.begin(),
                                                        replication_indices.end()};
        *(component_pb->mutable_location()) = {location.begin(), location.end()};

        // Setup Attributes For Component PB
        FillProtobufAttributes(component_pb->mutable_attributes(), component_id,
                               component_instance->GetUniqueIdSuffix());

        // Get All Children which we need to setup
        auto publisher_port_ids = graphml_parser_->FindImmediateChildren("PublisherPortInstance",
                                                                         component_id);
        auto subscriber_port_ids = graphml_parser_->FindImmediateChildren("SubscriberPortInstance",
                                                                          component_id);
        auto requester_port_ids = graphml_parser_->FindImmediateChildren("RequesterPortInstance",
                                                                         component_id);
        auto replier_port_ids = graphml_parser_->FindImmediateChildren("ReplierPortInstance",
                                                                       component_id);
        auto periodic_ids = graphml_parser_->FindImmediateChildren("PeriodicPortInstance",
                                                                   component_id);
        auto class_instance_ids = graphml_parser_->FindImmediateChildren("ClassInstance",
                                                                         component_id);
        auto trigger_instance_ids = graphml_parser_->FindImmediateChildren("TriggerInstance",
                                                                           component_id);
        std::vector<std::string> pubsub_port_ids;
        std::vector<std::string> reqrep_port_ids;

        // Combine Publisher and Subscriber Ports
        pubsub_port_ids.insert(pubsub_port_ids.end(), publisher_port_ids.begin(),
                               publisher_port_ids.end());
        pubsub_port_ids.insert(pubsub_port_ids.end(), subscriber_port_ids.begin(),
                               subscriber_port_ids.end());

        // Combine Requester and Replier Ports
        reqrep_port_ids.insert(reqrep_port_ids.end(), requester_port_ids.begin(),
                               requester_port_ids.end());
        reqrep_port_ids.insert(reqrep_port_ids.end(), replier_port_ids.begin(),
                               replier_port_ids.end());

        // Process Publisher/Subscriber Ports
        for(const auto& port_id : pubsub_port_ids) {
            auto port_pb = ConstructPubSubPortPb(port_id, unique_suffix);
            if(port_pb) {
                // Insert into the replicated port map
                port_replicate_id_map_[port_pb->info().id()] = port_pb.get();
                component_pb->mutable_ports()->AddAllocated(port_pb.release());
            }
        }

        // Process Requester/Replier Ports
        for(const auto& port_id : reqrep_port_ids) {
            auto port_pb = ConstructReqRepPortPb(port_id, unique_suffix);
            if(port_pb) {
                // Insert into the replicated port map
                port_replicate_id_map_[port_pb->info().id()] = port_pb.get();
                component_pb->mutable_ports()->AddAllocated(port_pb.release());
            }
        }

        // Process Periodic Ports
        for(const auto& periodic_id : periodic_ids) {
            auto port_pb = ConstructPeriodicPb(periodic_id, unique_suffix);
            if(port_pb) {
                component_pb->mutable_ports()->AddAllocated(port_pb.release());
            }
        }

        // Handle Class Instances
        for(const auto& class_instance_id : class_instance_ids) {
            auto worker_pb = ConstructWorkerPb(class_instance_id, unique_suffix);
            if(worker_pb) {
                component_pb->mutable_workers()->AddAllocated(worker_pb.release());
            }
        }

        // Process Trigger Instances
        for(const auto& trigger_instance_id : trigger_instance_ids) {
            FillTriggerPrototypePb(*component_pb->add_prototype_triggers(), trigger_instance_id,
                                   unique_suffix);
        }
    }
}

void ModelParser::Process()
{
    // Pre-condition checks
    assert(graphml_parser_);

    experiment_ = std::make_unique<NodeManager::Experiment>();
    experiment_->set_name(experiment_id_);

    ParseHardwareItems();
    ParseExternalDelegates();
    ParseLoggingClients();
    ParseLoggingServers();
    ParseComponents();

    CalculateReplication();
}

// Manual inspection of test models and test expected output is recommended when modifying this
// function. Find these in re/tests/re_common/graphmlparser/models. Each <test>.graphml file maps to
// the expected output contained in <test>.json
void ModelParser::CalculateReplication()
{
    // Calculate the connections taking into account replication!
    for(const auto& m : assembly_map_) {
        for(const auto& ac : m.second) {
            auto source_port_id = ac.source_id;
            auto target_port_id = ac.target_id;

            bool is_source_delegate = external_port_id_map_.count(source_port_id) > 0;
            bool is_target_delegate = external_port_id_map_.count(target_port_id) > 0;

            auto source_component_id = graphml_parser_->GetParentNode(source_port_id);
            auto target_component_id = graphml_parser_->GetParentNode(target_port_id);

            // Port to Port
            if(!is_source_delegate && !is_target_delegate) {
                if(component_replications_.count(source_component_id)
                   && component_replications_.count(target_component_id)) {
                    // Get all ComponentInstances from the Replication
                    for(auto source_component_instance_proto :
                        component_replications_[source_component_id]) {
                        const auto& source_replicate_indices =
                            source_component_instance_proto->replicate_indices();
                        auto s_unique = GetUniqueSuffix(
                            {source_replicate_indices.begin(), source_replicate_indices.end()});
                        auto s_uid = ac.source_id + s_unique;

                        NodeManager::Port* source_port_instance_proto = nullptr;
                        if(port_replicate_id_map_.count(s_uid)) {
                            source_port_instance_proto = port_replicate_id_map_[s_uid];
                        } else {
                            std::cerr << "Cant Find Source Port: " << s_uid << std::endl;
                            continue;
                        }

                        // If we are an inter_assembly edge, we need to connect every outeventport
                        // instance to every ineventport instance
                        if(ac.inter_assembly) {
                            // Connect to all!
                            // We dont actually want to connect to all, see model RE414 in
                            // test/re_common/graphmlparser/models. Only replicate connection if all
                            // parents down to and including shared parent have matching replication
                            // indices.
                            for(auto target_component_instance_proto :
                                component_replications_[target_component_id]) {
                                const auto& target_replicate_indices =
                                    target_component_instance_proto->replicate_indices();
                                auto t_unique = GetUniqueSuffix({target_replicate_indices.begin(),
                                                                 target_replicate_indices.end()});
                                auto t_uid = ac.target_id + t_unique;

                                auto shared_parent_id =
                                    graphml_parser_->GetSharedParent(ac.source_id, ac.target_id);
                                auto source_height = graphml_parser_->GetHeightToParent(
                                                         ac.source_id, shared_parent_id)
                                                     - 2;
                                auto target_height = graphml_parser_->GetHeightToParent(
                                                         ac.target_id, shared_parent_id)
                                                     - 2;

                                // Get the location of our shared parent in our replicate indices
                                // list.
                                auto source_index = source_replicate_indices.size() - source_height;
                                auto target_index = target_replicate_indices.size() - target_height;

                                // Our shared parent should be the same,
                                // therefore the index in our replicate indices that we want to
                                // check up till should be the same.
                                assert(source_index == target_index);

                                // Check that our parents match all the way down to our shared
                                // parent.
                                bool non_matching_parents = false;
                                for(int ii = 0; ii < source_index; ii++) {
                                    if(source_replicate_indices[ii]
                                       != target_replicate_indices[ii]) {
                                        non_matching_parents = true;
                                    }
                                }
                                // Have to check outside of loop s.t. we continue the right loop.
                                if(non_matching_parents) {
                                    continue;
                                }

                                // If our parent replications up to our shared parent match,
                                // find the target port instance and add both ends of the connection
                                NodeManager::Port* target_port_instance_proto = nullptr;
                                if(port_replicate_id_map_.count(t_uid)) {
                                    target_port_instance_proto = port_replicate_id_map_[t_uid];
                                } else {
                                    std::cerr << "Cant Find Target Port: " << t_uid << std::endl;
                                    continue;
                                }
                                if(source_port_instance_proto && target_port_instance_proto) {
                                    source_port_instance_proto->add_connected_ports(t_uid);
                                    target_port_instance_proto->add_connected_ports(s_uid);
                                }
                            }
                        } else {
                            // If contained in an assembly, we only need to replicate the one
                            // outeventport to the matching replication ineventport instance
                            auto t_uid = ac.target_id + s_unique;

                            NodeManager::Port* target_port_instance_proto = nullptr;

                            if(port_replicate_id_map_.count(t_uid)) {
                                target_port_instance_proto = port_replicate_id_map_[t_uid];
                            } else {
                                std::cerr << "Cant Find Target Port: " << t_uid << std::endl;
                                continue;
                            }

                            if(source_port_instance_proto && target_port_instance_proto) {
                                source_port_instance_proto->add_connected_ports(t_uid);
                                target_port_instance_proto->add_connected_ports(s_uid);
                            }
                        }
                    }
                }
            } else if(is_source_delegate && !is_target_delegate) {
                auto source_eport_proto = external_port_id_map_[source_port_id];

                if(component_replications_.count(target_component_id)) {
                    for(auto target_component_instance_proto :
                        component_replications_[target_component_id]) {
                        const auto& replicate_indices =
                            target_component_instance_proto->replicate_indices();
                        auto t_unique = GetUniqueSuffix(
                            {replicate_indices.begin(), replicate_indices.end()});
                        auto t_uid = target_port_id + t_unique;

                        NodeManager::Port* target_port_instance_proto = nullptr;
                        if(port_replicate_id_map_.count(t_uid)) {
                            target_port_instance_proto = port_replicate_id_map_[t_uid];
                        } else {
                            std::cerr << "Cant Find Target Port: " << t_uid << std::endl;
                            continue;
                        }

                        // Append the connection to our list
                        if(source_eport_proto && target_port_instance_proto) {
                            source_eport_proto->add_connected_ports(t_uid);
                            target_port_instance_proto->add_connected_external_ports(
                                source_port_id);
                        }
                    }
                }
            } else if(!is_source_delegate && is_target_delegate) {
                auto target_eport_proto = external_port_id_map_[target_port_id];

                if(component_replications_.count(source_component_id)) {
                    for(auto source_component_instance_proto :
                        component_replications_[source_component_id]) {
                        const auto& replicate_indices =
                            source_component_instance_proto->replicate_indices();
                        auto s_unique = GetUniqueSuffix(
                            {replicate_indices.begin(), replicate_indices.end()});
                        auto s_uid = source_port_id + s_unique;

                        NodeManager::Port* source_port_instance_proto = nullptr;
                        if(port_replicate_id_map_.count(s_uid)) {
                            source_port_instance_proto = port_replicate_id_map_[s_uid];
                        } else {
                            std::cerr << "Cant Find Source Port: " << s_uid << std::endl;
                            continue;
                        }

                        // Append the connection to our list
                        if(source_port_instance_proto && target_eport_proto) {
                            source_port_instance_proto->add_connected_external_ports(
                                target_port_id);
                            target_eport_proto->add_connected_ports(s_uid);
                        }
                    }
                }
            }
        }
    }
}

void ModelParser::SetAttributePb(NodeManager::Attribute& attr_pb,
                                 const std::string& type,
                                 const std::string& value)
{
    NodeManager::Attribute::Kind kind;
    if(type == "Integer") {
        kind = NodeManager::Attribute::INTEGER;
    } else if(type == "Boolean") {
        kind = NodeManager::Attribute::BOOLEAN;
    } else if(type == "Character") {
        kind = NodeManager::Attribute::CHARACTER;
    } else if(type == "String") {
        kind = NodeManager::Attribute::STRING;
    } else if(type == "Double") {
        kind = NodeManager::Attribute::DOUBLE;
    } else if(type == "Float") {
        kind = NodeManager::Attribute::FLOAT;
    } else {
        std::cerr << "Unhandled Graphml Attribute Type: '" << type << "'" << std::endl;
        kind = NodeManager::Attribute::STRING;
    }
    attr_pb.set_kind(kind);
    switch(kind) {
        case NodeManager::Attribute::FLOAT:
        case NodeManager::Attribute::DOUBLE: {
            double double_val = 0;
            try {
                double_val = std::stod(value);
            } catch(const std::invalid_argument&) {
                double_val = 0;
            }
            attr_pb.set_d(double_val);
            break;
        }
        case NodeManager::Attribute::CHARACTER: {
            // need to trim out ' characters
            auto char_str = value;
            char_str.erase(std::remove(char_str.begin(), char_str.end(), '\''), char_str.end());
            if(char_str.length() == 1) {
                attr_pb.set_i(char_str[0]);
            } else {
                std::cerr << "Character: '" << value << "' isn't length one!" << std::endl;
            }
            break;
        }
        case NodeManager::Attribute::BOOLEAN: {
            bool val = str2bool(value);
            attr_pb.set_i(val);
            break;
        }
        case NodeManager::Attribute::INTEGER: {
            int int_val = 0;
            try {
                int_val = std::stoi(value);
            } catch(const std::invalid_argument&) {
                int_val = 0;
            }
            attr_pb.set_i(int_val);
            break;
        }
        case NodeManager::Attribute::STRING: {
            auto str = value;
            str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
            attr_pb.add_s(str);
            break;
        }
        default:
            std::cerr << "Got unhandled Attribute type: "
                      << NodeManager::Attribute_Kind_Name(attr_pb.kind()) << std::endl;
            break;
    }
}

std::string ModelParser::GetDefinitionId(const std::string& id)
{
    if(definition_ids_.count(id)) {
        return definition_ids_[id];
    }
    std::string def_id;

    // Get the Definition ID of the
    for(const auto& e_id : definition_edge_ids_) {
        auto target = graphml_parser_->GetAttribute(e_id, "target");
        auto source = graphml_parser_->GetAttribute(e_id, "source");

        if(source == id) {
            def_id = target;
            break;
        }
    }

    if(!def_id.empty()) {
        // Save our Definition so we don't have to recalculate it
        definition_ids_[id] = def_id;
    }
    return def_id;
}

std::string ModelParser::GetAggregateId(const std::string& id)
{
    if(aggregate_ids_.count(id)) {
        return aggregate_ids_[id];
    }

    std::string agg_id;

    for(const auto& e_id : aggregate_edge_ids_) {
        auto target = graphml_parser_->GetAttribute(e_id, "target");
        auto source = graphml_parser_->GetAttribute(e_id, "source");

        if(source == id) {
            agg_id = target;
            break;
        }
    }

    if(!agg_id.empty()) {
        aggregate_ids_[id] = agg_id;
    }
    return agg_id;
}

NodeManager::Port::Kind ModelParser::GetPortKind(const std::string& kind) const
{
    if(kind == "PublisherPortInstance") {
        return NodeManager::Port::PUBLISHER;
    } else if(kind == "SubscriberPortInstance") {
        return NodeManager::Port::SUBSCRIBER;
    } else if(kind == "PeriodicPortInstance") {
        return NodeManager::Port::PERIODIC;
    } else if(kind == "RequesterPortInstance") {
        return NodeManager::Port::REQUESTER;
    } else if(kind == "ReplierPortInstance") {
        return NodeManager::Port::REPLIER;
    }
    std::cerr << "INVALID PORT KIND: " << kind << std::endl;
    return NodeManager::Port::NO_KIND;
}

NodeManager::ExternalPort::Kind ModelParser::GetExternalPortKind(const std::string& kind) const
{
    if(kind == "PubSubPortDelegate") {
        return NodeManager::ExternalPort::PUBSUB;
    } else if(kind == "ServerPortDelegate") {
        return NodeManager::ExternalPort::SERVER;
    } else if(kind == "RequestPortDelegate") {
        return NodeManager::ExternalPort::SERVER;
    } else if(kind == "ReplyPortDelegate") {
        return NodeManager::ExternalPort::SERVER;
    }
    return NodeManager::ExternalPort::NO_KIND;
}

bool ModelParser::str2bool(std::string str) const
{
    try {
        return (bool)(std::stoi(str));
    } catch(const std::invalid_argument&) {
    }
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

void ModelParser::FillProtobufAttributes(
    ::google::protobuf::Map<::std::string, ::NodeManager::Attribute>* attrs,
    const std::string& parent_id,
    const std::string& unique_id_suffix)
{
    for(const auto& attribute_id :
        graphml_parser_->FindImmediateChildren("AttributeInstance", parent_id)) {
        const auto& attr_label = graphml_parser_->GetDataValue(attribute_id, "label");

        auto& attr_pb = NodeManager::InsertAttribute(attrs, attr_label,
                                                     NodeManager::Attribute::STRING);
        attr_pb.mutable_info()->set_id(attribute_id + unique_id_suffix);

        SetAttributePb(attr_pb, graphml_parser_->GetDataValue(attribute_id, "type"),
                       attribute_value_map_[attribute_id]);
    }
}

std::list<std::string> ModelParser::GetNamespaceList(const std::string& id) const
{
    std::list<std::string> namespace_list;
    auto current_id = id;

    while(!current_id.empty()) {
        const auto& kind = graphml_parser_->GetDataValue(current_id, "kind");
        if(kind == "Namespace") {
            const auto& label = graphml_parser_->GetDataValue(current_id, "label");
            namespace_list.emplace_front(label);
        }
        current_id = graphml_parser_->GetParentNode(current_id);
    }
    return namespace_list;
}

std::string ModelParser::GetUniqueSuffix(const std::vector<int>& indices)
{
    std::stringstream ss;
    for(const auto& id : indices) {
        ss << "_" << id;
    }
    return ss.str();
}

NodeManager::Middleware ModelParser::ParseMiddleware(const std::string& middleware_str) const
{
    auto middleware = NodeManager::NO_MIDDLEWARE;
    if(!NodeManager::Middleware_Parse(middleware_str, &middleware)) {
        std::cerr << "Cannot parse middleware: " << middleware_str << std::endl;
    }
    return middleware;
}

std::unique_ptr<NodeManager::Port>
ModelParser::ConstructPubSubPortPb(const std::string& port_id, const std::string& unique_id_suffix)
{
    auto port_pb = std::make_unique<NodeManager::Port>();
    auto aggregate_id = GetAggregateId(GetDefinitionId(port_id));

    auto port_info_pb = port_pb->mutable_info();
    port_info_pb->set_id(port_id + unique_id_suffix);
    port_info_pb->set_name(graphml_parser_->GetDataValue(port_id, "label"));
    port_info_pb->set_type(graphml_parser_->GetDataValue(aggregate_id, "label"));

    // Copy in the new namespaces
    for(const auto& ns : GetNamespaceList(aggregate_id)) {
        port_info_pb->add_namespaces(ns);
    }

    port_pb->set_kind(GetPortKind(graphml_parser_->GetDataValue(port_id, "kind")));

    // Set Middleware
    auto middleware = ParseMiddleware(graphml_parser_->GetDataValue(port_id, "middleware"));
    port_pb->set_middleware(middleware);

    if(middleware == NodeManager::RTI || middleware == NodeManager::OSPL
       || middleware == NodeManager::QPID) {
        // Set the topic_name
        const auto& topic_name = graphml_parser_->GetDataValue(port_id, "topic_name");
        NodeManager::SetStringAttribute(port_pb->mutable_attributes(), "topic_name", topic_name);
    }
    return port_pb;
}

std::unique_ptr<NodeManager::Port>
ModelParser::ConstructReqRepPortPb(const std::string& port_id, const std::string& unique_id_suffix)
{
    auto port_pb = std::make_unique<NodeManager::Port>();

    auto server_id = GetAggregateId(GetDefinitionId(port_id));

    auto port_info_pb = port_pb->mutable_info();

    port_info_pb->set_id(port_id + unique_id_suffix);
    port_info_pb->set_name(graphml_parser_->GetDataValue(port_id, "label"));
    port_info_pb->set_type(graphml_parser_->GetDataValue(server_id, "label"));

    // Copy in the new namespaces
    for(const auto& ns : GetNamespaceList(server_id)) {
        port_info_pb->add_namespaces(ns);
    }

    port_pb->set_kind(GetPortKind(graphml_parser_->GetDataValue(port_id, "kind")));

    // Set Middleware
    auto middleware = ParseMiddleware(graphml_parser_->GetDataValue(port_id, "middleware"));
    port_pb->set_middleware(middleware);

    if(middleware == NodeManager::QPID) {
        // Set the topic_name
        const auto& topic_name = graphml_parser_->GetDataValue(port_id, "topic_name");
        NodeManager::SetStringAttribute(port_pb->mutable_attributes(), "topic_name", topic_name);
    }
    return port_pb;
}

std::unique_ptr<NodeManager::Port>
ModelParser::ConstructPeriodicPb(const std::string& port_id, const std::string& unique_id_suffix)
{
    auto port_pb = std::make_unique<NodeManager::Port>();
    auto port_info_pb = port_pb->mutable_info();

    port_info_pb->set_id(port_id + unique_id_suffix);
    port_info_pb->set_name(graphml_parser_->GetDataValue(port_id, "label"));
    port_pb->set_kind(NodeManager::Port::PERIODIC);

    // Fill the Attributes
    FillProtobufAttributes(port_pb->mutable_attributes(), port_id, unique_id_suffix);
    return port_pb;
}

std::unique_ptr<NodeManager::Worker>
ModelParser::ConstructWorkerPb(const std::string& worker_id, const std::string& unique_id_suffix)
{
    const auto& class_type = graphml_parser_->GetDataValue(worker_id, "type");

    // Ignore vector operations
    if(class_type == "Vector_Operations" || class_type == "Component_Info") {
        return nullptr;
    }

    auto worker_pb = std::make_unique<NodeManager::Worker>();
    auto worker_info_pb = worker_pb->mutable_info();

    worker_info_pb->set_id(worker_id + unique_id_suffix);
    worker_info_pb->set_name(graphml_parser_->GetDataValue(worker_id, "label"));
    worker_info_pb->set_type(graphml_parser_->GetDataValue(worker_id, "type"));

    // Fill the Attributes
    FillProtobufAttributes(worker_pb->mutable_attributes(), worker_id, unique_id_suffix);
    return worker_pb;
}

void ModelParser::FillTriggerPrototypePb(NodeManager::TriggerPrototype& trigger_pb,
                                                 const std::string& trigger_inst_id,
                                                 const std::string& unique_id_suffix)
{
    const auto& trigger_definition_id = GetDefinitionId(trigger_inst_id);
    const auto& trigger_type = graphml_parser_->GetDataValue(trigger_definition_id, "trigger_type");
    if (trigger_type == "CPU_util") {
        trigger_pb.set_metric(NodeManager::TriggerPrototype::CPU_UTIL);
    } else if (trigger_type == "MEM_util") {
        trigger_pb.set_metric(NodeManager::TriggerPrototype::MEM_UTIL);
    } else {
        throw std::runtime_error("Invalid value for trigger key 'trigger-type': "+ trigger_type);
    }


    const auto& comparator_str = graphml_parser_->GetDataValue(trigger_definition_id, "condition");
    if (comparator_str == "==") {
        trigger_pb.set_comparator(NodeManager::TriggerPrototype::EQUAL);
    } else if (comparator_str == "!=") {
        trigger_pb.set_comparator(NodeManager::TriggerPrototype::NOT_EQUAL);
    } else if (comparator_str == "<") {
        trigger_pb.set_comparator(NodeManager::TriggerPrototype::LESS);
    } else if (comparator_str == ">") {
        trigger_pb.set_comparator(NodeManager::TriggerPrototype::GREATER);
    } else if (comparator_str == "<=") {
        trigger_pb.set_comparator(NodeManager::TriggerPrototype::LESS_OR_EQUAL);
    } else if (comparator_str == ">=") {
        trigger_pb.set_comparator(NodeManager::TriggerPrototype::GREATER_OR_EQUAL);
    } else {
        throw std::runtime_error("Invalid value for trigger key 'comparator': "+ comparator_str);
    }

    try {
        trigger_pb.set_value(
            std::stod(graphml_parser_->GetDataValue(trigger_definition_id, "value"))
        );
    } catch (std::invalid_argument& iae) {
        throw std::invalid_argument("Unable to parse double while building trigger value");
    }

    auto trigger_info_pb = trigger_pb.mutable_info();
    if (trigger_info_pb == nullptr) {
        throw std::runtime_error("Unable to construct/access mutable info protobuf object for a"
                                 "TriggerPrototype (returned null)");
    }
    trigger_info_pb->set_id(trigger_inst_id + unique_id_suffix);
    trigger_info_pb->set_name(graphml_parser_->GetDataValue(trigger_inst_id, "label"));
    trigger_info_pb->set_type(graphml_parser_->GetDataValue(trigger_inst_id, "type"));

    auto strat_instance_ids = graphml_parser_->FindImmediateChildren("StrategyInstance", trigger_inst_id);
    for (const auto& strategy_instance_id : strat_instance_ids) {
        FillStrategyPrototypePb(*trigger_pb.mutable_strategy(), strategy_instance_id, unique_id_suffix);
    }
}

void ModelParser::FillStrategyPrototypePb(NodeManager::StrategyPrototype& strategy_pb,
                                                  const std::string& strat_inst_id,
                                                  const std::string& unique_suffix) {
    auto strat_info = strategy_pb.mutable_info();
    if (strat_info == nullptr) {
        throw std::runtime_error("Unable to construct/access mutable info protobuf object for a StrategyPrototype (returned null)");
    }
    strat_info->set_id(strat_inst_id + unique_suffix);
    strat_info->set_name(graphml_parser_->GetDataValue(strat_inst_id, "label"));
    strat_info->set_type(graphml_parser_->GetDataValue(strat_inst_id, "type"));


    auto container_id = trigger_edge_ids_[strat_inst_id];

    strategy_pb.set_container_id(container_id);
}




NodeManager::Experiment& ModelParser::GetExperiment()
{
    return *experiment_;
}

} // namespace re::ModelParser
