#include "nodemanagerprotohandler.h"

#include "utils.h"

#include "databaseclient.h"
#include "experimenttracker.h"

#include <zmq/protoreceiver/protoreceiver.h>

#include <proto/controlmessage/helper.h>

#include <functional>

#include <zmq/zmqutils.hpp>


void NodeManagerProtoHandler::BindCallbacks(zmq::ProtoReceiver& receiver) {
    receiver.RegisterProtoCallback<NodeManager::EnvironmentMessage>(
        std::bind(&NodeManagerProtoHandler::ProcessEnvironmentMessage, this, std::placeholders::_1)
    );
}



void NodeManagerProtoHandler::ProcessEnvironmentMessage(const NodeManager::EnvironmentMessage& message) {
    switch (message.type()) {
        case NodeManager::EnvironmentMessage::CONFIGURE_EXPERIMENT:
            ProcessConfigureControlMessage(message.control_message());
            return;
        case NodeManager::EnvironmentMessage::SHUTDOWN_EXPERIMENT:
            ProcessShutdownControlMessage(message.control_message());
            return;
        default:
            throw std::logic_error("No handling implemented for EnvironmentMessage of type " + NodeManager::EnvironmentMessage::Type_Name(message.type()));
    }
}

void NodeManagerProtoHandler::ProcessGetInfoControlMessage(const NodeManager::ControlMessage& message) {
    // TODO: Handle GET_EXPERIMENT_INFO properly
    throw std::logic_error("GET_EXPERIMENT_INFO handling not implemented");
}


void NodeManagerProtoHandler::ProcessConfigureControlMessage(const NodeManager::ControlMessage& message) {
    switch(message.type()) {
        case NodeManager::ControlMessage::CONFIGURE: {
            int experiment_run_id = experiment_tracker_.RegisterExperimentRun(message.experiment_id(), message.timestamp());
            
            for (const auto& node : message.nodes()) {
                ProcessNode(node, experiment_run_id);
            }
            break;
        }
        default:
            throw std::logic_error(
                "No handling implemented during CONFIGURE for ControlMessage of type "+NodeManager::ControlMessage::Type_Name(message.type())
            ); 
    }
}

void NodeManagerProtoHandler::ProcessShutdownControlMessage(const NodeManager::ControlMessage& message) {
    switch(message.type()){
        case NodeManager::ControlMessage::NO_TYPE:
            experiment_tracker_.ShutdownExperimentRun(message.experiment_id(), message.timestamp());
            break;
        default:
            throw std::logic_error(
                "No handling implemented during SHUTDOWN_EXPERIMENT for ControlMessage of type "+NodeManager::ControlMessage::Type_Name(message.type())
            ); 
    }
}

void NodeManagerProtoHandler::ProcessNode(const NodeManager::Node& message, int experiment_run_id) {
    std::string hostname = message.info().name();
    std::string ip = message.ip_address();
    std::string graphml_id = message.info().id();

    int node_id = database_->InsertValuesUnique(
        "Node",
        {"ExperimentRunID", "IP", "Hostname", "GraphmlID"},
        {std::to_string(experiment_run_id), ip, hostname, graphml_id},
        {"IP", "ExperimentRunID"}
    );

    for (const auto& container : message.containers()) {
        ProcessContainer(container, experiment_run_id, node_id);
    }
}

void NodeManagerProtoHandler::ProcessContainer(const NodeManager::Container& message, int experiment_run_id, int node_id) {
    
    std::string name = message.info().name();
    std::string graphml_id = message.info().id();
    std::string type = NodeManager::Container::ContainerType_Name(message.type());
    
    int container_id = database_->InsertValuesUnique(
        "Container",
        {"NodeID", "Name", "GraphmlID", "Type"},
        {std::to_string(node_id), name, graphml_id, type},
        {"NodeID", "GraphmlID"}
    );

    for (const auto& component : message.components()) {
        ProcessComponent(component, experiment_run_id, container_id);
    }

    for (const auto& logger : message.loggers()) {
        ProcessLogger(logger, experiment_run_id);
    }
}

void NodeManagerProtoHandler::ProcessLogger(const NodeManager::Logger& message, int experiment_run_id) {
    const auto& endpoint = zmq::TCPify(message.publisher_address(), message.publisher_port());
    switch (message.type()) {
        case NodeManager::Logger::CLIENT:{
            experiment_tracker_.RegisterSystemEventProducer(experiment_run_id, endpoint);
            break;
        }
        case NodeManager::Logger::MODEL:{//Logger_Type::Logger_Type_MODEL:{
            experiment_tracker_.RegisterModelEventProducer(experiment_run_id, endpoint);
            break;
        }
        default:
            std::cout << "Disregarding logger of type " << NodeManager::Logger::Type_Name(message.type()) << std::endl;
            break;
    }
}

void NodeManagerProtoHandler::ProcessComponent(const NodeManager::Component& message, int experiment_run_id, int container_id) {
    if (message.location_size() != message.replicate_indices_size()) {
            // NOTE: sizes dont match
            throw std::runtime_error(std::string("Mismatch in size of replication and location vectors for component ").append(message.info().name()));
    }

    int component_id = database_->InsertValuesUnique(
        "Component",
        {"Name", "ExperimentRunID", "GraphmlID"},
        {message.info().type(), std::to_string(experiment_run_id), message.info().id()},
        {"Name", "ExperimentRunID"}
    );

    std::string full_location;
    auto&& location_vec = std::vector<std::string>(message.location().begin(), message.location().end());
    auto&& replication_vec = std::vector<int>(message.replicate_indices().begin(), message.replicate_indices().end());
    full_location = AggServer::GetFullLocation(location_vec, replication_vec, message.info().name());

    int component_instance_id = database_->InsertValuesUnique(
        "ComponentInstance",
        {"ComponentID", "Path", "Name", "ContainerID", "GraphmlID"},
        {std::to_string(component_id), full_location, message.info().name(), std::to_string(container_id), message.info().id()},
        {"Path", "ComponentID"} // Unique path per componentID -> Unique path per ExperimentRunID
    );

    for(const auto& port : message.ports()) {
        ProcessPort(port, component_instance_id, full_location);
    }
    
    for(const auto& worker : message.workers()) {
        ProcessWorker(worker, experiment_run_id, component_instance_id, full_location);
    }
}

void NodeManagerProtoHandler::ProcessPort(const NodeManager::Port& message, int component_instance_id, const std::string& component_instance_location) {

    //const std::string&& location = GetFullLocation(message.location(), message.replication_indices());
    const std::string port_path = component_instance_location + "/" + message.info().name();
    const std::string& port_kind = NodeManager::Port_Kind_Name(message.kind());
    const std::string& port_type = message.info().type();
    const std::string& middleware = NodeManager::Middleware_Name(message.middleware());

    int port_id = database_->InsertValuesUnique(
        "Port",
        {"Name", "ComponentInstanceID", "Path", "Kind", "Type", "Middleware", "GraphmlID"},
        {message.info().name(), std::to_string(component_instance_id), port_path, port_kind, port_type, middleware, message.info().id()},
        {"Name", "ComponentInstanceID"}
    );
}

void NodeManagerProtoHandler::ProcessWorker(const NodeManager::Worker& message, int experiment_run_id, int component_id, const std::string& component_path) {

    int worker_id = database_->InsertValuesUnique(
        "Worker",
        {"Name", "ExperimentRunID", "GraphmlID"},
        {message.info().type(), std::to_string(experiment_run_id), message.info().id()},
        {"Name", "ExperimentRunID"}
    );

    std::string worker_path = component_path + "/" + message.info().name();

    int worker_instance_id = database_->InsertValuesUnique(
        "WorkerInstance",
        {"Name", "WorkerID", "ComponentInstanceID", "Path", "GraphmlID"},
        {message.info().name(), std::to_string(worker_id), std::to_string(component_id), worker_path, message.info().id()},
        {"Name", "ComponentInstanceID"}
    );
}
