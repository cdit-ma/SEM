#include "experiment.h"
#include "container.h"
#include "environment.h"
#include "node.h"
#include "ports/port.h"
#include <algorithm>
#include "../experimentmanager.h"
#include <google/protobuf/util/time_util.h>
#include <memory>
#include "epmstdout.pb.h"
#include "publisher.hpp"
#include "helper.h"
#include <utility>

namespace re::EnvironmentManager {

Experiment::Experiment(Environment& environment,
                       NodeManagerRegistry& node_manager_registry,
                       types::Uuid uuid,
                       std::string name,
                       re::Representation::ExperimentDefinition experiment_definition) :
    environment_(environment),
    node_manager_registry_{node_manager_registry},
    experiment_definition_{std::move(experiment_definition)},
    experiment_manager_publisher_endpoint_(environment_.GetManagerEndpoint()),
    experiment_manager_registration_endpoint_(environment_.GetManagerEndpoint()),
    experiment_logger_endpoint_(environment_.GetManagerEndpoint()),
    experiment_uuid_{uuid},
    experiment_name_{std::move(name)},
    epm_registration_replier_{environment_.GetAmqpBrokerAddress(),
                              BuildEpmRegistrationTopic(experiment_uuid_), ""},
    trigger_handler_{environment_.GetAmqpBrokerAddress(),
                     experiment_uuid_.to_string() + "_triggers", ""}
{
    epm_registration_replier_.run([this](const EpmRegistrationRequest& request) {
        return HandleDockerEpmRegistration(request);
    });
    trigger_handler_.run([this](const TriggerEvent& event) { return HandleTriggerEvent(event); });
}

Experiment::~Experiment()
{
    try {
        for(const auto& external_port_pair : external_port_map_) {
            const auto& port = external_port_pair.second;
            const auto& external_port_label = port->external_label;

            if(!port->consumer_ids.empty()) {
                environment_.RemoveExternalConsumerPort(experiment_uuid_, external_port_label);
            }
            if(!port->producer_ids.empty()) {
                environment_.RemoveExternalProducerPort(experiment_uuid_, external_port_label);
            }
        }

        for(const auto& epm_uuid : epm_list_) {
            try {
                node_manager_registry_.StopEpm(epm_uuid);
                node_manager_registry_.KillBareMetalEpm(epm_uuid);
            } catch(const std::exception& ex) {
                LogError({"Failed to stop epm: {" + epm_uuid.to_string() + "}"});
            }
        }
        using MessageType = re::network::protocol::epmstdout::Message;
        re::network::Publisher<MessageType> publisher{environment_.GetAmqpBrokerAddress(),
                                                      experiment_uuid_.to_string() + "_epm_std_out",
                                                      ""};
        MessageType message;
        message.mutable_shutdown();
        publisher.publish(message);

        // Delete all nodes, frees all ports in destructors
        node_map_.clear();

        if(GetState() == ExperimentState::ACTIVE) {
            environment_.FreeManagerEndpoint(experiment_manager_publisher_endpoint_);
            environment_.FreeManagerEndpoint(experiment_manager_registration_endpoint_);
        }
    } catch(...) {
        LogError({"Could not delete deployment: " + experiment_name_});
    }
}

const std::string& Experiment::GetName() const
{
    return experiment_name_;
}

Environment& Experiment::GetEnvironment() const
{
    return environment_;
}

void Experiment::SetConfigured()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(state_ == ExperimentState::REGISTERED) {
        state_ = ExperimentState::CONFIGURED;
    } else {
        throw std::runtime_error("Invalid state transition (REGISTERED -> ![CONFIGURED]");
    }
}

void Experiment::SetActive()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(state_ == ExperimentState::CONFIGURED) {
        state_ = ExperimentState::ACTIVE;
    } else {
        throw std::runtime_error("Invalid state transition (CONFIGURED -> ![ACTIVE]");
    }
}

void Experiment::SetFinished()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(state_ == ExperimentState::ACTIVE) {
        state_ = ExperimentState::FINISHED;
    } else {
        throw std::runtime_error("Invalid state transition (ACTIVE -> ![FINISHED]");
    }
}

bool Experiment::IsConfigured() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return state_ == ExperimentState::CONFIGURED;
}

bool Experiment::IsRegistered() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return state_ == ExperimentState::REGISTERED;
}

bool Experiment::IsActive() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return state_ == ExperimentState::ACTIVE;
}

auto Experiment::IsFinished() const -> bool
{
    std::lock_guard lock(mutex_);
    return state_ == ExperimentState::FINISHED;
}

void Experiment::CheckValidity() const
{
    // Experiment validity checks
    // Check that we actually have components deployed in this experiment
    auto component_count = 0;
    for(const auto& node_pair : node_map_) {
        component_count += node_pair.second->GetDeployedComponentCount();
    }
    if(component_count == 0) {
        throw std::invalid_argument("Experiment: '" + experiment_name_
                                    + "' deploys no components.");
    }
}

void Experiment::AddExternalPorts(const NodeManager::Experiment& message)
{
    for(const auto& external_port : message.external_ports()) {
        if(!external_port.is_blackbox()) {
            const auto& internal_id = external_port.info().id();
            if(!external_port_map_.count(internal_id)) {
                auto port = std::make_unique<ExternalPort>();
                port->internal_id = internal_id;
                port->external_label = external_port.info().name();
                port->type = external_port.info().type();
                port->middleware = Port::TranslateProtoMiddleware(external_port.middleware());

                if(external_port.kind() == NodeManager::ExternalPort::PUBSUB) {
                    port->kind = ExternalPort::Kind::PubSub;
                }
                if(external_port.kind() == NodeManager::ExternalPort::SERVER) {
                    port->kind = ExternalPort::Kind::ReqRep;
                }

                external_port_map_.emplace(internal_id, std::move(port));
                external_id_to_internal_id_map_[external_port.info().name()] = internal_id;
            } else {
                throw std::invalid_argument("Experiment: '" + experiment_name_
                                            + "' Got duplicate external port id: '" + internal_id
                                            + "'");
            }
        } else {
            const auto& internal_id = external_port.info().id();
            auto port = Port::ConstructBlackboxPort(*this, external_port);
            if(port) {
                blackbox_port_map_.emplace(internal_id, std::move(port));
            }
        }
    }
}

Experiment::ExperimentState Experiment::GetState() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

Node& Experiment::GetNode(const std::string& ip_address) const
{
    try {
        return *(node_map_.at(ip_address));
    } catch(const std::exception&) {
        throw std::invalid_argument("Experiment: '" + experiment_name_
                                    + "' does not have registered node with IP: '" + ip_address
                                    + "'");
    }
}

void Experiment::AddNode(const NodeManager::Node& node)
{
    const auto& ip_address = node.ip_address();

    if(!node_map_.count(ip_address)) {
        auto internal_node = std::make_unique<EnvironmentManager::Node>(environment_, *this, node);
        node_map_.emplace(ip_address, std::move(internal_node));
    } else {
        throw std::invalid_argument("Experiment: '" + experiment_name_
                                    + "' Got duplicate node with ip address: '" + ip_address + "'");
    }
}

bool Experiment::HasDeploymentOn(const std::string& ip_address) const
{
    return GetNode(ip_address).GetDeployedCount() > 0;
}

std::unique_ptr<NodeManager::RegisterExperimentReply> Experiment::GetDeploymentInfo()
{
    auto reply = std::make_unique<NodeManager::RegisterExperimentReply>();
    reply->set_experiment_uuid(experiment_uuid_.to_string());

    for(const auto& node_pair : node_map_) {
        auto& node = (*node_pair.second);

        bool should_add = false;
        auto node_deployment = std::make_unique<NodeManager::NodeDeployment>();

        if(node.GetDeployedComponentCount()) {
            auto hardware_id = node.GetHardwareId();
            auto container_ids = node.GetComponentContainerIds();

            node_deployment->set_allocated_id(hardware_id.release());
            for(auto& container_id : container_ids) {
                node_deployment->mutable_container_ids()->AddAllocated(container_id.release());
            }
            should_add = true;
        }

        if(node.GetLoganServerCount()) {
            auto hardware_id = node.GetHardwareId();
            node_deployment->set_allocated_id(hardware_id.release());
            node_deployment->set_has_logan_server(true);
            should_add = true;
        }

        if(should_add) {
            reply->mutable_deployments()->AddAllocated(node_deployment.release());
        }
    }
    return reply;
}

auto Experiment::GetExperimentManagerPublisherEndpoint() -> types::SocketAddress
{
    return experiment_manager_publisher_endpoint_;
}

auto Experiment::GetExperimentManagerRegistrationEndpoint() -> types::SocketAddress
{
    return experiment_manager_registration_endpoint_;
}

bool Experiment::IsDirty() const
{
    return dirty_;
}

void Experiment::SetDirty()
{
    dirty_ = true;
}

void Experiment::Shutdown()
{
    if(!shutdown_) {
        shutdown_ = true;
        SetDirty();
    }
}

void Experiment::UpdatePort(const std::string& external_port_label)
{
    // Only Update configured/Active ports
    if(IsConfigured() || IsActive()) {
        if(external_id_to_internal_id_map_.count(external_port_label)) {
            const auto& internal_id = external_id_to_internal_id_map_.at(external_port_label);

            const auto& external_port = GetExternalPort(internal_id);

            for(const auto& port_id : external_port.consumer_ids) {
                GetPort(port_id).SetDirty();
            }
        }
    }
}

Port& Experiment::GetPort(const std::string& id)
{
    for(const auto& node_pair : node_map_) {
        if(node_pair.second->HasPort(id)) {
            return node_pair.second->GetPort(id);
        }
    }
    for(const auto& port_pair : blackbox_port_map_) {
        if(port_pair.first == id) {
            return *port_pair.second;
        }
    }
    throw std::out_of_range("Experiment::GetPort: <" + id + "> OUT OF RANGE");
}

std::unique_ptr<NodeManager::EnvironmentMessage> Experiment::GetProto(const bool full_update)
{
    std::unique_ptr<NodeManager::EnvironmentMessage> environment_message;

    if(dirty_ || full_update) {
        environment_message = std::make_unique<NodeManager::EnvironmentMessage>();

        if(!shutdown_) {
            environment_message->set_type(NodeManager::EnvironmentMessage::CONFIGURE_EXPERIMENT);
            auto control_message = environment_message->mutable_control_message();
            control_message->set_type(NodeManager::ControlMessage::CONFIGURE);

            control_message->set_experiment_id(experiment_name_);
            for(auto& node_pair : node_map_) {
                if(node_pair.second->GetDeployedComponentCount()) {
                    auto node_pb = node_pair.second->GetProto(full_update);
                    if(node_pb) {
                        control_message->mutable_nodes()->AddAllocated(node_pb.release());
                    }
                }
            }

            using namespace google::protobuf::util;
            auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
            auto timestamp = TimeUtil::MillisecondsToTimestamp(current_time.count());
            control_message->mutable_timestamp()->Swap(&timestamp);

            auto attrs = control_message->mutable_attributes();
            NodeManager::SetStringAttribute(
                attrs, "master_ip_address",
                GetExperimentManagerPublisherEndpoint().ip().to_string());
            NodeManager::SetStringAttribute(attrs, "master_publisher_endpoint",
                                            GetExperimentManagerPublisherEndpoint().to_string());
            NodeManager::SetStringAttribute(attrs, "master_registration_endpoint",
                                            GetExperimentManagerRegistrationEndpoint().to_string());
        } else {
            // Terminate the experiment
            environment_message->set_type(NodeManager::EnvironmentMessage::SHUTDOWN_EXPERIMENT);
            auto control_message = environment_message->mutable_control_message();
            control_message->set_experiment_id(experiment_name_);
        }

        if(dirty_) {
            dirty_ = false;
        }
    }
    return environment_message;
}

std::vector<std::unique_ptr<NodeManager::Logger>>
Experiment::GetAllocatedLoganServers(const std::string& ip_address)
{
    return std::move(GetNode(ip_address).GetAllocatedLoganServers());
}

Experiment::ExternalPort& Experiment::GetExternalPort(const std::string& external_port_internal_id)
{
    if(external_port_map_.count(external_port_internal_id)) {
        return *external_port_map_.at(external_port_internal_id);
    } else {
        throw std::invalid_argument("Experiment: '" + experiment_name_
                                    + "' doesn't have external port id: '"
                                    + external_port_internal_id + "'");
    }
}

void Experiment::AddExternalConsumerPort(const std::string& external_port_internal_id,
                                         const std::string& internal_port_id)
{
    try {
        auto& external_port = GetExternalPort(external_port_internal_id);
        environment_.AddExternalConsumerPort(experiment_uuid_, external_port.external_label);
        external_port.consumer_ids.insert(internal_port_id);
    } catch(const std::exception&) {
    }
}

void Experiment::AddExternalProducerPort(const std::string& external_port_internal_id,
                                         const std::string& internal_port_id)
{
    try {
        auto& external_port = GetExternalPort(external_port_internal_id);
        environment_.AddExternalProducerPort(experiment_uuid_, external_port.external_label);
        external_port.producer_ids.insert(internal_port_id);
    } catch(const std::exception&) {
    }
}

void Experiment::RemoveExternalConsumerPort(const std::string& external_port_internal_id,
                                            const std::string& internal_port_id)
{
    try {
        auto& external_port = GetExternalPort(external_port_internal_id);
        environment_.RemoveExternalConsumerPort(experiment_uuid_, external_port.external_label);
        external_port.consumer_ids.erase(internal_port_id);
    } catch(const std::exception&) {
    }
}

void Experiment::RemoveExternalProducerPort(const std::string& external_port_internal_id,
                                            const std::string& internal_port_id)
{
    try {
        auto& external_port = GetExternalPort(external_port_internal_id);
        environment_.RemoveExternalProducerPort(experiment_uuid_, external_port.external_label);
        external_port.producer_ids.erase(internal_port_id);
    } catch(const std::exception&) {
    }
}

std::vector<std::reference_wrapper<Port>>
Experiment::GetExternalProducerPorts(const std::string& external_port_label)
{
    std::vector<std::reference_wrapper<Port>> producer_ports;

    try {
        const auto& internal_port_id = GetExternalPortInternalId(external_port_label);

        const auto& external_port = GetExternalPort(internal_port_id);
        for(const auto& port_id : external_port.producer_ids) {
            producer_ports.emplace_back(GetPort(port_id));
        }
    } catch(const std::runtime_error&) {
    }

    return producer_ports;
}

std::vector<std::reference_wrapper<Logger>>
Experiment::GetLoggerClients(const std::string& logger_id)
{
    std::vector<std::reference_wrapper<Logger>> loggers;

    for(auto& node_pair : node_map_) {
        auto node_loggers = node_pair.second->GetLoggers(logger_id);
        loggers.insert(loggers.end(), node_loggers.begin(), node_loggers.end());
    }

    return loggers;
}

std::string Experiment::GetExternalPortLabel(const std::string& internal_port_id)
{
    auto& external_port = GetExternalPort(internal_port_id);
    return external_port.external_label;
}

std::string Experiment::GetExternalPortInternalId(const std::string& external_port_label)
{
    if(external_id_to_internal_id_map_.count(external_port_label)) {
        return external_id_to_internal_id_map_.at(external_port_label);
    }
    throw std::runtime_error("Experiment: '" + experiment_name_
                             + "' doesn't have an external port with label '" + external_port_label
                             + "'");
}

Node& Experiment::GetLeastDeployedToNode(bool non_empty)
{
    // Find node with fewest deployed components, if non_empty, require at least 1 component to be
    // the minimum
    return *(
        std::min_element(node_map_.cbegin(), node_map_.cend(), [non_empty](auto& p1, auto& p2) {
            const auto& p1_count = p1.second->GetDeployedComponentCount();
            const auto& p2_count = p2.second->GetDeployedComponentCount();

            if(non_empty) {
                if(p1_count == 0) {
                    return false;
                } else if(p2_count == 0) {
                    return true;
                }
            }
            return p1_count < p2_count;
        })->second);
}

void Experiment::AddLoggingClientToNodes(const NodeManager::Logger& logging_client)
{
    for(const auto& node : node_map_) {
        node.second->AddLoggingClient(logging_client);
    }
}

std::vector<std::reference_wrapper<Experiment::ExternalPort>> Experiment::GetExternalPorts() const
{
    std::vector<std::reference_wrapper<Experiment::ExternalPort>> external_ports;

    for(auto& port_pair : external_port_map_) {
        external_ports.emplace_back(*(port_pair.second));
    }
    return external_ports;
}

std::string Experiment::GetMessage() const
{
    using NodeMapPair = std::pair<const std::string, std::unique_ptr<Node>>;

    std::stringstream message_stream;
    std::string experiment_name = "* Experiment[" + experiment_name_ + "] ";

    for(const auto& node : node_map_) {
        if(node.second->GetDeployedComponentCount() > 0) {
            message_stream << experiment_name << node.second->GetMessage() << "\n";
        }
    }

    return message_stream.str();
}

types::SocketAddress Experiment::GetExperimentLoggerEndpoint()
{
    return experiment_logger_endpoint_;
}

auto Experiment::StartExperimentManager() -> void
{
    // Start an ExperimentManager to coordinate ExperimentProcesses
    experiment_manager_ = std::make_unique<ExperimentManager>(*this, duration_);
}

void Experiment::SetDuration(const types::Timeout& duration)
{
    duration_ = duration;
}

auto Experiment::StartDockerProcess(types::Ipv4 node_ip) -> std::pair<types::Uuid, std::string>
{
    auto request_uuid = types::Uuid{};
    // TODO: Fix this hard code.
    Docker client{"http://" + node_ip.to_string() + ":4000"};
    JSON_DOCUMENT param = GetDockerJsonRequest(node_ip, request_uuid);

    // Start the docker container that should run an experiment process manager.
    auto doc = client.create_container(param);

    if(!doc.HasMember("success")) {
        throw std::runtime_error("Docker API fault.");
    }

    if(doc["success"].GetBool()) {
        auto docker_container_id = std::string(doc["data"]["Id"].GetString());
        client.start_container(docker_container_id);
        return {WaitForEpmRegistrationMessage(request_uuid), docker_container_id};
    } else {
        throw std::runtime_error("Failed to start docker container.");
    }
}

JSON_DOCUMENT
Experiment::GetDockerJsonRequest(const types::Ipv4& node_ip, const types::Uuid& request_uuid)
{
    JSON_DOCUMENT param(rapidjson::kObjectType);

    // Add our image name param
    auto& allocator = param.GetAllocator();
    param.AddMember("Image", "cdit-ma/re_minimal", allocator);

    // Build our Cmd argument array
    JSON_VALUE commands(rapidjson::kArrayType);
    commands.PushBack("/re/bin/experiment_process_manager", allocator);
    auto docker_args = CreateDockerCommandString(node_ip, request_uuid);
    for(const auto& arg : docker_args) {
        JSON_VALUE str;
        str.SetString(arg, allocator);
        commands.PushBack(str, allocator);
    }
    param.AddMember("Cmd", commands, allocator);

    // Build our host config object
    // Build our network settings
    JSON_VALUE host_config(rapidjson::kObjectType);
    host_config.AddMember("NetworkMode", "host", allocator);

    // Build our bind directories
    JSON_VALUE binds(rapidjson::kArrayType);
    JSON_VALUE lib_bind;
    // TODO: FIX THIS HARD CODE
    std::string lib_bind_string{"/home/cdit-ma/model_ws/lib:/experiment_libs"};
    lib_bind.SetString(lib_bind_string, allocator);
    binds.PushBack(lib_bind, allocator);
    host_config.AddMember("Binds", binds, allocator);

    param.AddMember("HostConfig", host_config, param.GetAllocator());
    return param;
}

auto Experiment::HandleDockerEpmRegistration(const EpmRegistrationRequest& request)
    -> EpmRegistrationReply
{
    // TODO: Build list of pending registration uuids. Check against that before we notify.
    auto request_uuid = types::Uuid{request.request_uuid()};
    auto epm_uuid = types::Uuid{request.epm_uuid()};
    {
        std::lock_guard lock{epm_registration_mutex_};
        epm_registrations_.push({request_uuid, epm_uuid});
    }
    epm_registration_semaphore_.notify_one();
    EpmRegistrationReply reply;
    reply.set_success(true);
    return reply;
}

auto Experiment::WaitForEpmRegistrationMessage(const types::Uuid& request_uuid) -> types::Uuid
{
    // How long we should wait for the EPM to start
    // TODO: Tune this number?
    auto registration_wait_timeout = std::chrono::milliseconds(5000);

    std::unique_lock lock{epm_registration_mutex_};
    if(epm_registration_semaphore_.wait_for(lock, registration_wait_timeout, [this, request_uuid]() {
           // Check that we have a registration to process and
           // that the registration id matches.
           //  XXX: This could be a problem?
           return !epm_registrations_.empty()
                  && epm_registrations_.front().request_uuid == request_uuid;
       })) {
        auto epm_uuid = epm_registrations_.front().epm_uuid;
        epm_registrations_.pop();

        return epm_uuid;
    } else {
        throw std::runtime_error("EPM startup timed out.");
    }
}

auto Experiment::StartExperimentProcesses() -> void
{
    std::vector<types::Uuid> epm_list;
    try {
        for(const auto& [id, node] : node_map_) {
            for(const auto& [id, container] : node->GetContainers()) {
                if(container->GetDeployedComponentCount() == 0) {
                    // Skip containers without deployed components.
                    continue;
                }
                epm_list.push_back(StartEpmForContainer(*node, *container));
            }
        }
        epm_list_ = epm_list;
    } catch(const std::exception& ex) {
        LogError({"Failure standing up EPMs for: " + experiment_name_, ex.what()});

        // Stop the Epm's we've started.
        for(const auto& epm_uuid : epm_list) {
            try {
                node_manager_registry_.StopEpm(epm_uuid);
                node_manager_registry_.KillBareMetalEpm(epm_uuid);
                KillDockerEpm(epm_uuid);
            } catch(const std::exception& ex) {
                LogError({ex.what()});
            }
        }
        throw;
    }
}
types::Uuid Experiment::StartEpmForContainer(EnvironmentManager::Node& node,
                                             EnvironmentManager::Container& container)
{
    if(container.IsDocker()) {
        auto [docker_epm_uuid, docker_container_id] = StartDockerProcess(node.GetIp());

        // Start an experiment process on the docker EPM
        node_manager_registry_.StartExperimentProcess(docker_epm_uuid, container.GetId(),
                                                      GetExperimentManagerPublisherEndpoint(),
                                                      GetExperimentManagerRegistrationEndpoint(),
                                                      "/experiment_libs");
        return docker_epm_uuid;
    } else {
        // Currently starting an epm per experiment process.
        // This will change in the future after a rework of DeploymentManager into
        //  ExperimentProcess.
        const auto node_uuid = node_manager_registry_.GetNodeManagerUuid(node.GetName());
        auto epm_uuid = node_manager_registry_.NewEpm(node_uuid, experiment_uuid_,
                                                      experiment_name_);

        node_manager_registry_.StartExperimentProcess(epm_uuid, container.GetId(),
                                                      GetExperimentManagerPublisherEndpoint(),
                                                      GetExperimentManagerRegistrationEndpoint(),

                                                      // TODO: Standardise library location
                                                      //  Fix to have experiment uuid prefixed.
                                                      experiment_uuid_.to_string() + "/lib");
        return epm_uuid;
    }
}

std::vector<std::string>
Experiment::CreateDockerCommandString(types::Ipv4 node_ip, types::Uuid creation_request_uuid)
{
    std::vector<std::string> out;
    out.push_back("--experiment_uuid=" + experiment_uuid_.to_string());
    out.push_back("--creation_request_uuid=" + creation_request_uuid.to_string());
    out.push_back("--qpid_broker_endpoint=" + environment_.GetAmqpBrokerAddress().to_string());
    out.push_back("--ip_address=" + node_ip.to_string());
    out.emplace_back("--lib_root_dir=/");
    out.emplace_back("--re_bin_dir=/re/bin");
    out.push_back("--registration_entity_uuid=" + experiment_uuid_.to_string());
    return out;
}

std::string Experiment::BuildEpmRegistrationTopic(types::Uuid experiment_uuid)
{
    return experiment_uuid.to_string() + "_epm_registration";
}

auto Experiment::KillDockerEpm(types::Uuid epm_uuid) -> void
{
    // TODO: Implement. At the moment, we're leaking docker containers.
}

void Experiment::HandleTriggerEvent(const TriggerEvent& event)
{
    Log({"Received trigger id: " + event.trigger_id()});
    const auto& trigger_id = event.trigger_id();

    if(!TriggerShouldFire(trigger_id)) {
        return;
    }

    auto container = experiment_definition_.GetContainerToStartOnTriggerEvent(trigger_id);
    auto deployment_location = experiment_definition_.GetDeploymentLocation(container);

    types::Uuid epm_uuid;
    types::Uuid event_uuid{};
    try {
        if(std::holds_alternative<re::Representation::Node>(deployment_location)) {
            auto& node_to_deploy_to = GetNode(
                std::get<re::Representation::Node>(deployment_location).GetIpAddress().to_string());
            auto& new_container = node_to_deploy_to.AddContainer(event_uuid, experiment_definition_,
                                                                 container);
            // force experiment manager to refresh its model defintion
            experiment_manager_->UpdateExperiment();
            epm_uuid = StartEpmForContainer(node_to_deploy_to, new_container);
            experiment_manager_->PushStateChange(NodeManager::ControlMessage::ACTIVATE);

        } else if(std::holds_alternative<re::Representation::Cluster>(deployment_location)) {
            auto& least_deployed_to_node = GetLeastDeployedToNode(true);
            auto& new_container =
                least_deployed_to_node.AddContainer(event_uuid, experiment_definition_, container);
            // force experiment manager to refresh its model defintion
            experiment_manager_->UpdateExperiment();
            epm_uuid = StartEpmForContainer(least_deployed_to_node, new_container);
            experiment_manager_->PushStateChange(NodeManager::ControlMessage::ACTIVATE);
        }
    } catch(const std::exception& ex) {
        LogError({"Failure standing up EPMs for: " + experiment_name_, ex.what()});
        // Stop the Epm we've started.
        try {
            node_manager_registry_.StopEpm(epm_uuid);
            node_manager_registry_.KillBareMetalEpm(epm_uuid);
            KillDockerEpm(epm_uuid);
        } catch(const std::exception& ex) {
            LogError({ex.what()});
        }
    }
}
re::EnvironmentManager::TriggerPrototype& Experiment::GetTrigger(const std::string& trigger_id) {}

auto Experiment::Log(const std::vector<std::string>& messages) const -> void
{
    std::cout << FormatLogMessage(messages) << std::endl;
}

auto Experiment::LogError(const std::vector<std::string>& messages) const -> void
{
    std::cerr << FormatLogMessage(messages) << std::endl;
}

auto Experiment::FormatLogMessage(const std::vector<std::string>& messages) const -> std::string
{
    std::string output;
    output = "[Experiment]         - {" + GetUuid().to_string() + "} " + messages.front();
    if(messages.size() == 1) {
        return output;
    }

    // Use annoying for loop to start from second element
    for(auto iter = std::begin(messages) + 1, end = std::end(messages); iter != end; iter++) {
        output += ("\n                     - " + *iter);
    }
    return output;
}
bool Experiment::TriggerShouldFire(const std::string& trigger_id)
{
    auto now = std::chrono::system_clock::now();
    auto trigger_instance = experiment_definition_.GetTriggerInstFromMedeaId(trigger_id);
    auto trigger_definition = experiment_definition_.GetTriggerDefinition(trigger_instance);
    auto instance_uuid = trigger_instance.GetCoreData().GetUuid();

    // Bail out early if we're a single shot trigger that has fired before.
    if(trigger_definition.IsSingleActivation() && trigger_fire_times_.count(instance_uuid) != 0) {
        return false;
    }

    if(trigger_fire_times_.count(instance_uuid) != 0) {
        // We've fired before, check when. Update time if we need to, return.
        auto cooldown = trigger_definition.GetReactivationCooldown();
        auto last_fire_time = trigger_fire_times_.at(instance_uuid);
        auto time_since_last_fire = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - last_fire_time);

        if(time_since_last_fire.count() > cooldown.count()) {
            // If more time than 'cooldown' has passed since last_fire_time, update time.
            trigger_fire_times_[instance_uuid] = now;
            return true;
        } else {
            return false;
        }
    }

    // First time trigger has fired, add to map.
    trigger_fire_times_.emplace(instance_uuid, now);
    return true;
}

} // namespace re::EnvironmentManager
