#include "experiment.h"
#include "environment.h"
#include "node.h"
#include "container.h"
#include "ports/port.h"
#include "helper.h"
#include <algorithm>
#include <numeric>
#include <google/protobuf/util/time_util.h>

using namespace EnvironmentManager;

Experiment::Experiment(Environment &environment, const std::string name) : environment_(environment) {
    model_name_ = name;
}

Experiment::~Experiment() {
    try {
        for (const auto &external_port_pair : external_port_map_) {
            const auto &port = external_port_pair.second;
            const auto &external_port_label = port->external_label;

            if (!port->consumer_ids.empty()) {
                environment_.RemoveExternalConsumerPort(model_name_, external_port_label);
            }
            if (!port->producer_ids.empty()) {
                environment_.RemoveExternalProducerPort(model_name_, external_port_label);
            }
        }

        //Delete all nodes, frees all ports in destructors
        node_map_.clear();

        environment_.FreeManagerPort(manager_port_);

        if (GetState() == ExperimentState::ACTIVE) {
            environment_.FreePort(master_ip_address_, master_publisher_port_);
            environment_.FreePort(master_ip_address_, master_registration_port_);
        }
    }
    catch (...) {
        std::cerr << "Could not delete deployment :" << model_name_ << std::endl;
    }
}

const std::string &Experiment::GetName() const {
    return model_name_;
}

Environment &Experiment::GetEnvironment() const {
    return environment_;
}

void Experiment::SetConfigured() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (state_ == ExperimentState::Registered) {
        state_ = ExperimentState::CONFIGURED;
    } else {
        throw std::runtime_error("Invalid state transition (REGISTERED -> ![CONFIGURED]");
    }
}

void Experiment::SetActive() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (state_ == ExperimentState::CONFIGURED) {
        state_ = ExperimentState::ACTIVE;
    } else {
        throw std::runtime_error("Invalid state transition (CONFIGURED -> ![ACTIVE]");
    }
}

bool Experiment::IsConfigured() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return state_ == ExperimentState::CONFIGURED;
}

bool Experiment::IsRegistered() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return state_ == ExperimentState::Registered;
}

bool Experiment::IsActive() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return state_ == ExperimentState::ACTIVE;
}

void Experiment::CheckValidity() const {
    //Experiment validity checks

    //Check that we actually have components deployed in this experiment
    auto component_count = 0;
    for(const auto& node_pair : node_map_){
        component_count += node_pair.second->GetDeployedComponentCount();
    }
    if(component_count == 0){
        throw std::invalid_argument("Experiment: '" + model_name_ +"' deploys no components.");
    }

    //Check that we have a master container assigned
    auto has_node_manager_master = false;
    for(const auto& node_pair : node_map_){
        if(node_pair.second->HasNodeManagerMaster()){
            has_node_manager_master = true;
            break;
        }
    }
    if(!has_node_manager_master) {
        throw std::runtime_error("Experiment: '" + model_name_ +"' has no container set as node manager master.");
    }

    //Check that we have the information required to set up communication with the node manager master
    if(master_ip_address_.empty()){
        throw std::runtime_error("Experiment: '" + model_name_ +"' has no master ip address.");
    }
    if(manager_port_.empty()){
        throw std::runtime_error("Experiment: '" + model_name_ +"' has no master manager port.");
    }

    //Check that we have ports assigned for the node manager master to communicate with node manager slaves
    if(master_publisher_port_.empty()){
        throw std::runtime_error("Experiment: '" + model_name_ +"' has no master publisher port.");
    }
    if(master_registration_port_.empty()){
        throw std::runtime_error("Experiment: '" + model_name_ +"' has no master registration port.");
    }

}

std::string Experiment::GetManagerPort() const {
    return manager_port_;
}

void Experiment::SetManagerPort(const std::string &manager_port) {
    manager_port_ = manager_port;
}

void Experiment::ConfigureMaster() {
    //Set up our master container. This will be the container that manages stand-up of all child/slave containers
    for (const auto &node : node_map_) {
        if (node.second->HasMasterEligibleContainer()) {
            node.second->GetMasterEligibleContainer().SetNodeManagerMaster();
            SetMasterIp(node.second->GetIp());
            GetMasterPublisherAddress();
            GetMasterRegistrationAddress();
            break;
        }
    }
}

void Experiment::SetMasterIp(const std::string &ip) {
    master_ip_address_ = ip;
}

void Experiment::AddExternalPorts(const NodeManager::Experiment &message) {
    for (const auto &external_port : message.external_ports()) {
        if (!external_port.is_blackbox()) {
            const auto &internal_id = external_port.info().id();
            if (!external_port_map_.count(internal_id)) {
                auto port = std::unique_ptr<ExternalPort>(new ExternalPort());
                port->internal_id = internal_id;
                port->external_label = external_port.info().name();
                port->type = external_port.info().type();
                port->middleware = Port::TranslateProtoMiddleware(external_port.middleware());

                if (external_port.kind() == NodeManager::ExternalPort::PUBSUB) {
                    port->kind = ExternalPort::Kind::PubSub;
                }
                if (external_port.kind() == NodeManager::ExternalPort::SERVER) {
                    port->kind = ExternalPort::Kind::ReqRep;
                }

                external_port_map_.emplace(internal_id, std::move(port));
                external_id_to_internal_id_map_[external_port.info().name()] = internal_id;
            } else {
                throw std::invalid_argument(
                        "Experiment: '" + model_name_ + "' Got duplicate external port id: '" + internal_id + "'");
            }
        } else {
            const auto &internal_id = external_port.info().id();
            auto port = Port::ConstructBlackboxPort(*this, external_port);
            if (port) {
                blackbox_port_map_.emplace(internal_id, std::move(port));
            }
        }
    }
}

Experiment::ExperimentState Experiment::GetState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

Node &Experiment::GetNode(const std::string &ip_address) const {
    try {
        return *(node_map_.at(ip_address));
    } catch (const std::exception &) {
        throw std::invalid_argument(
                "Experiment: '" + model_name_ + "' does not have registered node with IP: '" + ip_address + "'");
    }
}

void Experiment::AddNode(const NodeManager::Node &node) {
    const auto &ip_address = node.ip_address();

    if (!node_map_.count(ip_address)) {
        auto internal_node = std::unique_ptr<EnvironmentManager::Node>(
                new EnvironmentManager::Node(environment_, *this, node));
        node_map_.emplace(ip_address, std::move(internal_node));
    } else {
        throw std::invalid_argument(
                "Experiment: '" + model_name_ + "' Got duplicate node with ip address: '" + ip_address + "'");
    }
}

bool Experiment::HasDeploymentOn(const std::string &ip_address) const {
    return GetNode(ip_address).GetDeployedCount() > 0;
}

std::unique_ptr<NodeManager::RegisterExperimentReply> Experiment::GetDeploymentInfo() {
    auto reply = std::unique_ptr<NodeManager::RegisterExperimentReply>(new NodeManager::RegisterExperimentReply());

    for (const auto &node_pair : node_map_) {
        auto &node = (*node_pair.second);

        bool should_add = false;
        auto node_deployment = std::unique_ptr<NodeManager::NodeDeployment>(new NodeManager::NodeDeployment);

        if (node.GetDeployedComponentCount()) {
            auto hardware_id = node.GetHardwareId();
            auto container_ids = node.GetComponentContainerIds();

            node_deployment->set_allocated_id(hardware_id.release());
            for (auto &container_id : container_ids) {
                node_deployment->mutable_container_ids()->AddAllocated(container_id.release());
            }
            should_add = true;
        }

        if (node.GetLoganServerCount()) {
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

std::string Experiment::GetMasterPublisherAddress() {
    if (master_publisher_port_.empty()) {
        master_publisher_port_ = environment_.GetPort(master_ip_address_);
    }
    return "tcp://" + master_ip_address_ + ":" + master_publisher_port_;
}

const std::string &Experiment::GetMasterIp() const {
    return master_ip_address_;
}

std::string Experiment::GetMasterRegistrationAddress() {
    if (master_registration_port_.empty()) {
        master_registration_port_ = environment_.GetPort(master_ip_address_);
    }
    return "tcp://" + master_ip_address_ + ":" + master_registration_port_;
}

bool Experiment::IsDirty() const {
    return dirty_;
}

void Experiment::SetDirty() {
    dirty_ = true;
}

void Experiment::Shutdown() {
    if (!shutdown_) {
        shutdown_ = true;
        SetDirty();
    }
}

void Experiment::UpdatePort(const std::string &external_port_label) {
    //Only Update configured/Active ports
    if (IsConfigured() || IsActive()) {
        if (external_id_to_internal_id_map_.count(external_port_label)) {
            const auto &internal_id = external_id_to_internal_id_map_.at(external_port_label);

            const auto &external_port = GetExternalPort(internal_id);

            for (const auto &port_id : external_port.consumer_ids) {
                GetPort(port_id).SetDirty();
            }
        }
    }
}

Port &Experiment::GetPort(const std::string &id) {
    for (const auto &node_pair : node_map_) {
        if (node_pair.second->HasPort(id)) {
            return node_pair.second->GetPort(id);
        }
    }
    for (const auto &port_pair : blackbox_port_map_) {
        if (port_pair.first == id) {
            return *port_pair.second;
        }
    }
    throw std::out_of_range("Experiment::GetPort: <" + id + "> OUT OF RANGE");
}

std::unique_ptr<NodeManager::EnvironmentMessage> Experiment::GetProto(const bool full_update) {
    std::unique_ptr<NodeManager::EnvironmentMessage> environment_message;

    if (dirty_ || full_update) {
        environment_message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage());

        if (!shutdown_) {
            environment_message->set_type(NodeManager::EnvironmentMessage::CONFIGURE_EXPERIMENT);
            auto control_message = environment_message->mutable_control_message();
            control_message->set_type(NodeManager::ControlMessage::CONFIGURE);

            control_message->set_experiment_id(model_name_);
            for (auto &node_pair : node_map_) {
                if (node_pair.second->GetDeployedComponentCount()) {
                    auto node_pb = node_pair.second->GetProto(full_update);
                    if (node_pb) {
                        control_message->mutable_nodes()->AddAllocated(node_pb.release());
                    }
                }
            }

            using namespace google::protobuf::util;
            auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            auto timestamp = TimeUtil::MillisecondsToTimestamp(current_time.count());
            control_message->mutable_timestamp()->Swap(&timestamp);

            auto attrs = control_message->mutable_attributes();
            NodeManager::SetStringAttribute(attrs, "master_ip_address", GetMasterIp());
            NodeManager::SetStringAttribute(attrs, "master_publisher_endpoint", GetMasterPublisherAddress());
            NodeManager::SetStringAttribute(attrs, "master_registration_endpoint", GetMasterRegistrationAddress());
        } else {
            //Terminate the experiment
            environment_message->set_type(NodeManager::EnvironmentMessage::SHUTDOWN_EXPERIMENT);
            auto control_message = environment_message->mutable_control_message();
            control_message->set_experiment_id(model_name_);
        }

        if (dirty_) {
            dirty_ = false;
        }
    }
    return environment_message;
}

std::vector<std::unique_ptr<NodeManager::Logger> > Experiment::GetAllocatedLoganServers(const std::string &ip_address) {
    return std::move(GetNode(ip_address).GetAllocatedLoganServers());
}

Experiment::ExternalPort &Experiment::GetExternalPort(const std::string &external_port_internal_id) {
    if (external_port_map_.count(external_port_internal_id)) {
        return *external_port_map_.at(external_port_internal_id);
    } else {
        throw std::invalid_argument(
                "Experiment: '" + model_name_ + "' doesn't have external port id: '" + external_port_internal_id + "'");
    }
}

void
Experiment::AddExternalConsumerPort(const std::string &external_port_internal_id, const std::string &internal_port_id) {
    try {
        auto &external_port = GetExternalPort(external_port_internal_id);
        environment_.AddExternalConsumerPort(model_name_, external_port.external_label);
        external_port.consumer_ids.insert(internal_port_id);
    }
    catch (const std::exception &) {
    }
}

void
Experiment::AddExternalProducerPort(const std::string &external_port_internal_id, const std::string &internal_port_id) {
    try {
        auto &external_port = GetExternalPort(external_port_internal_id);
        environment_.AddExternalProducerPort(model_name_, external_port.external_label);
        external_port.producer_ids.insert(internal_port_id);
    }
    catch (const std::exception &) {
    }
}

void Experiment::RemoveExternalConsumerPort(const std::string &external_port_internal_id,
                                            const std::string &internal_port_id) {
    try {
        auto &external_port = GetExternalPort(external_port_internal_id);
        environment_.RemoveExternalConsumerPort(model_name_, external_port.external_label);
        external_port.consumer_ids.erase(internal_port_id);
    }
    catch (const std::exception &) {
    }
}

void Experiment::RemoveExternalProducerPort(const std::string &external_port_internal_id,
                                            const std::string &internal_port_id) {
    try {
        auto &external_port = GetExternalPort(external_port_internal_id);
        environment_.RemoveExternalProducerPort(model_name_, external_port.external_label);
        external_port.producer_ids.erase(internal_port_id);
    }
    catch (const std::exception &) {
    }
}

std::vector<std::reference_wrapper<Port> >
Experiment::GetExternalProducerPorts(const std::string &external_port_label) {
    std::vector<std::reference_wrapper<Port> > producer_ports;

    try {
        const auto &internal_port_id = GetExternalPortInternalId(external_port_label);

        const auto &external_port = GetExternalPort(internal_port_id);
        for (const auto &port_id : external_port.producer_ids) {
            producer_ports.emplace_back(GetPort(port_id));
        }
    } catch (const std::runtime_error &) {
    }

    return producer_ports;
}

std::vector<std::reference_wrapper<Logger> > Experiment::GetLoggerClients(const std::string &logger_id) {
    std::vector<std::reference_wrapper<Logger> > loggers;

    for (auto &node_pair : node_map_) {
        auto node_loggers = node_pair.second->GetLoggers(logger_id);
        loggers.insert(loggers.end(), node_loggers.begin(), node_loggers.end());
    }

    return loggers;
}

std::string Experiment::GetExternalPortLabel(const std::string &internal_port_id) {
    auto &external_port = GetExternalPort(internal_port_id);
    return external_port.external_label;
}

std::string Experiment::GetExternalPortInternalId(const std::string &external_port_label) {
    if (external_id_to_internal_id_map_.count(external_port_label)) {
        return external_id_to_internal_id_map_.at(external_port_label);
    }
    throw std::runtime_error(
            "Experiment: '" + model_name_ + "' doesn't have an external port with label '" + external_port_label + "'");
}

Node &Experiment::GetLeastDeployedToNode(bool non_empty) {
    // Min element with lambda comparison func, will be prettier in 14/17 with 'auto' lambda arguments
    using NodeMapPair = std::pair<const std::string, std::unique_ptr<Node> >;


    //Find node with fewest deployed components, if non_empty, require at least 1 component to be the minimum
    return *(std::min_element(node_map_.cbegin(), node_map_.cend(),
                            [non_empty](const NodeMapPair &p1, const NodeMapPair &p2) {
                                const auto& p1_count = p1.second->GetDeployedComponentCount();
                                const auto& p2_count = p2.second->GetDeployedComponentCount();
                                
                                if(non_empty){
                                    if(p1_count == 0){
                                        return false;
                                    }else if(p2_count == 0){
                                        return true;
                                    }
                                }
                                return p1_count < p2_count;
                              })->second);
}

void Experiment::AddLoggingClientToNodes(const NodeManager::Logger &logging_client) {
    for (const auto &node : node_map_) {
        node.second->AddLoggingClient(logging_client);
    }
}

std::vector<std::reference_wrapper<Experiment::ExternalPort>> Experiment::GetExternalPorts() const {
    std::vector<std::reference_wrapper<Experiment::ExternalPort> > external_ports;

    for (auto &port_pair : external_port_map_) {
        external_ports.emplace_back(*(port_pair.second));
    }
    return external_ports;
}

std::string Experiment::GetLogMessage() const {
    using NodeMapPair = std::pair<const std::string, std::unique_ptr<Node> >;

    std::stringstream message_stream;
    std::string experiment_name = "* Experiment[" + model_name_ + "] ";

    for(const auto& node : node_map_){
        if(node.second->GetDeployedComponentCount() > 0){
            message_stream << experiment_name << node.second->GetLogMessage() << "\n";
        }
    }

    return message_stream.str();
}
