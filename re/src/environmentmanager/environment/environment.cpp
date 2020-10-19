#include "environment.h"
#include "node.h"
#include <cassert>
#include "experimentdefinition.h"
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <memory>
#include "helper.h"
#include <regex>
#include <utility>
#include <vector>

namespace re::EnvironmentManager {
Environment::Environment(const types::Ipv4& ip_address,
                         const types::SocketAddress& qpid_broker_address,
                         std::string tao_naming_service_address,
                         types::unique_queue<uint16_t> manager_port_pool,
                         types::unique_queue<uint16_t> default_experiment_node_port_pool) :
    ip_address_(ip_address),
    qpid_broker_address_(qpid_broker_address),
    tao_naming_service_address_(std::move(tao_naming_service_address)),
    default_experiment_node_port_pool_{std::move(default_experiment_node_port_pool)},
    environment_manager_endpoint_pool_{"environment_manager", ip_address,
                                       std::move(manager_port_pool)},
    update_publisher_endpoint_(this->environment_manager_endpoint_pool_.GetEndpoint())
{
    assert(!environment_manager_endpoint_pool_.empty());
    assert(!default_experiment_node_port_pool_.empty());

    // TODO: Publish to update publisher when we have a new experiment or an update for an
    //  experiment.
    update_publisher_ = std::make_unique<zmq::ProtoWriter>();
    update_publisher_->BindPublisherSocket(update_publisher_endpoint_.tcp());
    std::cout << "[Environment]        - Started" << std::endl;
}

Environment::~Environment()
{
    // Force that anything that requires the Environment to teardown is destroyed
    experiment_map_.clear();
}

auto Environment::PopulateExperiment(NodeManagerRegistry& node_manager_registry,
                                     const NodeManager::Experiment& message,
                                     re::Representation::ExperimentDefinition experiment_definition)
    -> void
{
    // REVIEW (Mitch): Change this function with great care. Here be dragons.
    //  This function performs a multi stage setup of any given experiment. Each stage of this alg
    //  inadverntantly looks the experiment map.
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);

    const auto& experiment_name = message.name();
    const auto experiment_uuid = experiment_definition.GetUuid();
    if(experiment_map_.count(experiment_uuid) > 0) {
        throw std::runtime_error("Got duplicate experiment: '" + experiment_name + "'");
    }

    try {
        experiment_map_.emplace(experiment_uuid,
                                std::make_unique<EnvironmentManager::Experiment>(
                                    *this, node_manager_registry, experiment_uuid, experiment_name,
                                    std::move(experiment_definition)));
        auto& experiment = *experiment_map_.at(experiment_uuid);

        if(message.duration_seconds() == -1) {
            experiment.SetDuration(types::NeverTimeout());
        } else {
            experiment.SetDuration(std::chrono::milliseconds(message.duration_seconds() * 1000));
        }

        // Handle the External Ports
        experiment.AddExternalPorts(message);

        // Add all nodes
        for(const auto& cluster : message.clusters()) {
            AddNodes(experiment_uuid, cluster);
        }

        // Configure our master settings and check experiment validity
        experiment.CheckValidity();

        // Complete the Configuration
        experiment.SetConfigured();
        experiment.StartExperimentManager();

        // Send our experiment's configuration to anything interested in updates
        update_publisher_->PushMessage(experiment.GetProto(true));

    } catch(const std::exception& ex) {
        // Remove the Experiment if we have any exceptions
        std::cerr << "[Environment]        - Failed to configure experiment: " << experiment_name
                  << " {" << experiment_uuid << "}\n"
                  << "                     - " << ex.what() << "\n"
                  << "                     - Removing" << std::endl;
        RemoveExperimentInternal(experiment_uuid);
        throw;
    }
}

void Environment::AddNodes(const types::Uuid& experiment_uuid, const NodeManager::Cluster& cluster)
{
    for(const auto& node : cluster.nodes()) {
        AddNodeToExperiment(experiment_uuid, node);
    }

    auto& containers = cluster.containers();

    std::vector<std::reference_wrapper<const NodeManager::Container>> implicit_containers;

    for(const auto& container : cluster.containers()) {
        if(container.implicit()) {
            implicit_containers.emplace_back(container);
        } else {
            DeployContainer(experiment_uuid, container);
        }
    }
    for(const auto& container : implicit_containers) {
        DistributeContainerToImplicitNodeContainers(experiment_uuid, container);
    }
}

void Environment::DistributeContainerToImplicitNodeContainers(
    const types::Uuid& experiment_uuid, const NodeManager::Container& container)
{
    // will end up deployed to nodes not necessarily belonging to that cluster
    for(const auto& component : container.components()) {
        GetExperimentInternal(experiment_uuid)
            .GetLeastDeployedToNode()
            .AddComponentToImplicitContainer(component);
    }

    for(const auto& logger : container.loggers()) {
        if(logger.type() == NodeManager::Logger::CLIENT) {
            // Add client to all nodes implicit containers.
            GetExperimentInternal(experiment_uuid).AddLoggingClientToNodes(logger);
        }
        if(logger.type() == NodeManager::Logger::SERVER) {
            // Add server to the least deployed to node's implicit container
            GetExperimentInternal(experiment_uuid)
                .GetLeastDeployedToNode(true)
                .AddLoggingServerToImplicitContainer(logger);
        }
    }
}

void Environment::DeployContainer(const types::Uuid& experiment_uuid,
                                  const NodeManager::Container& container)
{
    GetExperimentInternal(experiment_uuid).GetLeastDeployedToNode().AddContainer(container);
}

auto Environment::GetDeploymentHandlerEndpoint(const types::Uuid& experiment_uuid,
                                               DeploymentType deployment_type)
    -> types::SocketAddress
{
    switch(deployment_type) {
        case DeploymentType::LOGAN_SERVER: {
            // Get a fresh port for the environment to service the LoganServer
            return GetManagerEndpoint();
        }
        default:
            throw std::runtime_error("Unexpected Deployment Type");
    }
}

Experiment& Environment::GetExperiment(const types::Uuid& experiment_uuid)
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    return GetExperimentInternal(experiment_uuid);
}

Experiment& Environment::GetExperimentInternal(const types::Uuid& experiment_uuid)
{
    if(experiment_map_.count(experiment_uuid)) {
        return *(experiment_map_.at(experiment_uuid));
    }
    throw std::invalid_argument("No registered experiments: '" + experiment_uuid.to_string() + "'");
}

std::unique_ptr<NodeManager::RegisterExperimentReply>
Environment::GetExperimentDeploymentInfo(const types::Uuid& experiment_uuid)
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperimentInternal(experiment_uuid);
    return experiment.GetDeploymentInfo();
}

std::unique_ptr<NodeManager::EnvironmentMessage>
Environment::GetProto(const types::Uuid& experiment_uuid, const bool full_update)
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    auto& experiment = GetExperimentInternal(experiment_uuid);

    auto proto = experiment.GetProto(full_update);

    // Create copy of our control_message to send to aggregation server.
    auto update_proto_control_message = std::make_unique<NodeManager::EnvironmentMessage>(*proto);
    update_publisher_->PushMessage(std::move(update_proto_control_message));

    return proto;
}

void Environment::ShutdownExperiment(const types::Uuid& experiment_uuid)
{
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);
    ShutdownExperimentInternal(experiment_uuid);
}

std::vector<types::Uuid> Environment::ShutdownExperimentRegex(const std::string& regex)
{
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);

    auto experiment_uuids = GetMatchingExperiments(regex);
    for(const auto& experiment_uuid : experiment_uuids) {
        ShutdownExperimentInternal(experiment_uuid);
    }
    return experiment_uuids;
}

void Environment::ShutdownExperimentInternal(const types::Uuid& experiment_uuid)
{
    auto& experiment = GetExperimentInternal(experiment_uuid);
    if(experiment.IsActive()) {
        experiment.Shutdown();
    } else {
        RemoveExperimentInternal(experiment_uuid);
    }
}

std::vector<types::Uuid>
Environment::GetMatchingExperiments(const std::string& experiment_name_regex)
{
    std::vector<types::Uuid> experiment_list;
    std::regex match_re(experiment_name_regex);

    for(const auto& e_pair : experiment_map_) {
        std::smatch match;
        auto experiment_name = e_pair.second->GetName();
        if(std::regex_match(experiment_name, match, match_re)) {
            experiment_list.emplace_back(e_pair.first);
        }
    }
    return experiment_list;
}

void Environment::RemoveExperiment(const types::Uuid& experiment_uuid)
{
    std::lock(configure_experiment_mutex_, experiment_mutex_);
    std::lock_guard<std::mutex> configure_lock(configure_experiment_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> experiment_lock(experiment_mutex_, std::adopt_lock);
    RemoveExperimentInternal(experiment_uuid);
}

void Environment::RemoveExperimentInternal(const types::Uuid& experiment_uuid)
{
    if(experiment_map_.count(experiment_uuid)) {
        auto experiment_name = experiment_map_[experiment_uuid]->GetName();
        experiment_map_.erase(experiment_uuid);
        std::cout << "[Environment]       - Removed: {" << experiment_uuid << "}\n"
                  << "                    - Current registered experiments: "
                  << experiment_map_.size() << std::endl;

        auto remove_experiment_message = std::make_unique<NodeManager::EnvironmentMessage>();
        remove_experiment_message->set_type(NodeManager::EnvironmentMessage::SHUTDOWN_EXPERIMENT);
        remove_experiment_message->mutable_control_message()->set_experiment_id(experiment_name);
        using namespace google::protobuf::util;
        auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        auto timestamp = TimeUtil::MillisecondsToTimestamp(current_time.count());
        remove_experiment_message->mutable_control_message()->mutable_timestamp()->Swap(&timestamp);
        update_publisher_->PushMessage(std::move(remove_experiment_message));
    }
}

void Environment::AddNodeToExperiment(const types::Uuid& experiment_uuid,
                                      const NodeManager::Node& node)
{
    auto& experiment = GetExperimentInternal(experiment_uuid);
    try {
        AddNodeToEnvironment(node);
        experiment.AddNode(node);
    } catch(const std::exception& ex) {
        std::cerr << "* AddNodeToExperiment: " << ex.what() << std::endl;
        throw;
    }
}

void Environment::AddNodeToEnvironment(const NodeManager::Node& node)
{
    const auto& node_name = node.info().name();
    try {
        const auto& ip_str = node.ip_address();
        auto ip = types::Ipv4(ip_str);
        std::lock_guard<std::mutex> lock(node_mutex_);
        if(!node_available_endpoint_map_.count(ip)) {
            auto port_tracker = std::make_unique<EnvironmentManager::EndpointTracker>(
                node_name, ip, default_experiment_node_port_pool_);
            node_ip_map_.insert({node_name, ip});
            node_name_map_.insert({ip, node_name});
            node_available_endpoint_map_.emplace(ip, std::move(port_tracker));
        }
    } catch(const std::exception& ex) {
        // REVIEW (Mitch): ?????
        // Ignore the exception
    }
}

/// Get new allocated endpoint for specified ip_address
auto Environment::GetEndpoint(types::Ipv4 ip_address) -> types::SocketAddress
{
    std::lock_guard<std::mutex> lock(node_mutex_);
    if(node_available_endpoint_map_.count(ip_address)) {
        // Get first available port, store then erase it
        return node_available_endpoint_map_.at(ip_address)->GetEndpoint();
    }
    throw std::invalid_argument("Environment::GetPort No node found with ip address: '"
                                + ip_address.to_string() + "'");
}

/// Free endpoint
auto Environment::FreeEndpoint(types::SocketAddress endpoint) -> void
{
    std::lock_guard<std::mutex> lock(node_mutex_);
    if(node_available_endpoint_map_.count(endpoint.ip())) {
        node_available_endpoint_map_.at(endpoint.ip())->FreeEndpoint(endpoint);
    }
}

auto Environment::GetManagerEndpoint() -> types::SocketAddress
{
    return environment_manager_endpoint_pool_.GetEndpoint();
}

void Environment::FreeManagerEndpoint(types::SocketAddress endpoint)
{
    environment_manager_endpoint_pool_.FreeEndpoint(endpoint);
}

bool Environment::NodeDeployedTo(const types::Uuid& experiment_uuid, const std::string& node_ip)
{
    try {
        std::lock_guard<std::mutex> lock(experiment_mutex_);
        return GetExperimentInternal(experiment_uuid).HasDeploymentOn(node_ip);
    } catch(const std::invalid_argument& ex) {
    }
    return false;
}

bool Environment::IsExperimentConfigured(const types::Uuid& experiment_uuid)
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    try {
        auto& experiment = GetExperimentInternal(experiment_uuid);
        return experiment.IsConfigured();
    } catch(const std::exception& ex) {
    }
    return false;
}

bool Environment::IsExperimentRegistered(const types::Uuid& experiment_uuid)
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    try {
        auto& experiment = GetExperimentInternal(experiment_uuid);
        return experiment.IsRegistered();
    } catch(const std::exception& ex) {
    }
    return false;
}

bool Environment::IsExperimentActive(const types::Uuid& experiment_uuid)
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    try {
        auto& experiment = GetExperimentInternal(experiment_uuid);
        return experiment.IsActive();
    } catch(const std::exception& ex) {
    }
    return false;
}

bool Environment::GotExperiment(const types::Uuid& experiment_uuid) const
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    return experiment_map_.count(experiment_uuid) > 0;
}

bool Environment::ExperimentIsDirty(const types::Uuid& experiment_uuid)
{
    std::lock_guard<std::mutex> lock(configure_experiment_mutex_);
    if(GotExperiment(experiment_uuid)) {
        return GetExperimentInternal(experiment_uuid).IsDirty();
    }
    return false;
}

types::SocketAddress Environment::GetAmqpBrokerAddress()
{
    return qpid_broker_address_;
}

std::string Environment::GetTaoNamingServiceAddress()
{
    return tao_naming_service_address_;
}

Environment::ExternalPort& Environment::GetExternalPort(const std::string& external_port_label)
{
    if(external_eventport_map_.count(external_port_label)) {
        return *external_eventport_map_[external_port_label];
    }
    throw std::runtime_error("Environment doesn't have an External Port with ID: '"
                             + external_port_label + "'");
}

std::vector<std::reference_wrapper<Port>>
Environment::GetExternalProducerPorts(const std::string& external_port_label)
{
    std::vector<std::reference_wrapper<Port>> producer_ports;
    const auto& external_port = GetExternalPort(external_port_label);

    std::vector<types::Uuid> producer_experiments(external_port.producer_experiments.begin(),
                                                  external_port.producer_experiments.end());
    // Sort the Experiment names
    // REVIEW (MITCH): WHY?
    // std::sort(producer_experiments.begin(), producer_experiments.end());
    for(const auto& experiment_uuid : producer_experiments) {
        auto& experiment = GetExperimentInternal(experiment_uuid);
        for(auto& port : experiment.GetExternalProducerPorts(external_port_label)) {
            producer_ports.emplace_back(port);
        }
    }
    return producer_ports;
}

void Environment::AddExternalConsumerPort(const types::Uuid& experiment_uuid,
                                          const std::string& external_port_label)
{
    if(!external_eventport_map_.count(external_port_label)) {
        auto external_port = new ExternalPort();
        external_port->external_label = external_port_label;
        external_eventport_map_.emplace(external_port_label,
                                        std::unique_ptr<ExternalPort>(external_port));
    }
    auto& external_port = GetExternalPort(external_port_label);
    std::cout << "* Experiment Name: '" << experiment_uuid << "' Consumes: '" << external_port_label
              << "'" << std::endl;
    external_port.consumer_experiments.insert(experiment_uuid);
}

void Environment::AddExternalProducerPort(const types::Uuid& experiment_uuid,
                                          const std::string& external_port_label)
{
    if(!external_eventport_map_.count(external_port_label)) {
        auto external_port = new ExternalPort();
        external_port->external_label = external_port_label;
        external_eventport_map_.emplace(external_port_label,
                                        std::unique_ptr<ExternalPort>(external_port));
    }
    auto& external_port = GetExternalPort(external_port_label);
    external_port.producer_experiments.insert(experiment_uuid);
    std::cout << "* Experiment Name: '" << experiment_uuid << "' Produces: '" << external_port_label
              << "'" << std::endl;

    // Update Consumers
    for(const auto& consumer_experiment_name : external_port.consumer_experiments) {
        auto& consumer_experiment = GetExperimentInternal(consumer_experiment_name);
        consumer_experiment.UpdatePort(external_port_label);
    }
}

void Environment::RemoveExternalConsumerPort(const types::Uuid& experiment_uuid,
                                             const std::string& external_port_label)
{
    auto& external_port = GetExternalPort(external_port_label);
    external_port.consumer_experiments.erase(experiment_uuid);
}

void Environment::RemoveExternalProducerPort(const types::Uuid& experiment_uuid,
                                             const std::string& external_port_label)
{
    auto& external_port = GetExternalPort(external_port_label);
    external_port.producer_experiments.erase(experiment_uuid);
    // Update Consumers
    for(const auto& consumer_experiment_name : external_port.consumer_experiments) {
        auto& consumer_experiment = GetExperimentInternal(consumer_experiment_name);
        consumer_experiment.UpdatePort(external_port_label);
    }
}

std::vector<std::string> Environment::GetExperimentNames() const
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    std::vector<std::string> experiment_names;
    for(const auto& key_pair : experiment_map_) {
        experiment_names.emplace_back(key_pair.second->GetName());
    }
    return experiment_names;
}

std::vector<std::unique_ptr<NodeManager::ExternalPort>> Environment::GetExternalPorts() const
{
    std::lock_guard<std::mutex> lock(experiment_mutex_);
    std::vector<std::unique_ptr<NodeManager::ExternalPort>> external_ports;

    for(auto& experiment_pair : experiment_map_) {
        auto& experiment = experiment_pair.second;

        for(auto& external_port : experiment->GetExternalPorts()) {
            auto port_message = std::make_unique<NodeManager::ExternalPort>();

            port_message->mutable_info()->set_name(external_port.get().external_label);
            port_message->mutable_info()->set_type(external_port.get().type);

            port_message->set_middleware(
                Port::TranslateInternalMiddleware(external_port.get().middleware));

            if(external_port.get().kind == Experiment::ExternalPort::Kind::PubSub) {
                port_message->set_kind(NodeManager::ExternalPort::PUBSUB);
            }
            if(external_port.get().kind == Experiment::ExternalPort::Kind::ReqRep) {
                port_message->set_kind(NodeManager::ExternalPort::SERVER);
            }
            external_ports.push_back(std::move(port_message));
        }
    }

    return external_ports;
}

auto Environment::GetUpdatePublisherEndpoint() const -> types::SocketAddress
{
    return update_publisher_endpoint_;
}

auto Environment::GetExperimentUuid(const std::string& name) -> types::Uuid
{
    for(const auto& experiment_pair : experiment_map_) {
        if(experiment_pair.second->GetName() == name) {
            return experiment_pair.first;
        }
    }
    throw std::out_of_range("No experiment found with name: " + name);
}
auto Environment::CleanUpExperiments() -> void
{
    std::vector<types::Uuid> to_remove;
    std::lock_guard lock(experiment_mutex_);
    for(const auto& experiment : experiment_map_) {
        if(experiment.second->IsFinished()) {
            to_remove.push_back(experiment.first);
        }
    }
    for(const auto& experiment_uuid : to_remove) {
        RemoveExperimentInternal(experiment_uuid);
    }
}
} // namespace re::EnvironmentManager