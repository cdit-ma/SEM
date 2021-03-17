#include "aggregationproxy.h"
#include "../SettingsController/settingscontroller.h"

#include "dummyresponsebuilder.h"

#include <QDateTime>
#include <iostream>

std::unique_ptr<AggregationProxy> AggregationProxy::proxySingleton_ = nullptr;

/**
 * @brief AggregationProxy::AggregationProxy
 * @throws NoRequesterException
 */
AggregationProxy::AggregationProxy()
{
    auto settings = SettingsController::settings();
    connect(settings, &SettingsController::settingChanged, [=](SETTINGS key, QVariant value) {
        if (key == SETTINGS::CHARTS_AGGREGATION_BROKER_ENDPOINT) {
            SetServerEndpoint(value.toString());
        }
    });

    SetServerEndpoint(settings->getSetting(SETTINGS::CHARTS_AGGREGATION_BROKER_ENDPOINT).toString());
}


/**
 * @brief AggregationProxy::singleton
 * @throws NoRequesterException
 * @return
 */
AggregationProxy& AggregationProxy::singleton()
{
    if (!proxySingleton_) {
        proxySingleton_ = std::unique_ptr<AggregationProxy>(new AggregationProxy());
    }
    return *proxySingleton_;
}


/**
 * @brief AggregationProxy::RequestExperimentRuns
 * @param experiment_name
 * @return
 */
QFuture<QVector<AggServerResponse::ExperimentRun>> AggregationProxy::RequestExperimentRuns(const QString& experiment_name) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetExperimentRuns, experiment_name);
}


/**
 * @brief AggregationProxy::RequestExperimentState
 * @param experiment_run_id
 * @return
 */
QFuture<AggServerResponse::ExperimentState> AggregationProxy::RequestExperimentState(const quint32 experiment_run_id) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetExperimentState, experiment_run_id);
}


/**
 * @brief AggregationProxy::RequestPortLifecycleEvents
 * @param request
 * @return
 */
QFuture< QVector<PortLifecycleEvent*> > AggregationProxy::RequestPortLifecycleEvents(const PortLifecycleRequest& request) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetPortLifecycleEvents, request);
}


/**
 * @brief AggregationProxy::RequestWorkloadEvents
 * @param request
 * @return
 */
QFuture< QVector<WorkloadEvent*> > AggregationProxy::RequestWorkloadEvents(const WorkloadRequest& request) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetWorkloadEvents, request);
}


/**
 * @brief AggregationProxy::RequestCPUUtilisationEvents
 * @param request
 * @return
 */
QFuture< QVector<CPUUtilisationEvent*> > AggregationProxy::RequestCPUUtilisationEvents(const HardwareMetricRequest& request) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetCPUUtilisationEvents, request);
}


/**
 * @brief AggregationProxy::RequestMemoryUtilisationEvents
 * @param request
 * @return
 */
QFuture< QVector<MemoryUtilisationEvent*> > AggregationProxy::RequestMemoryUtilisationEvents(const HardwareMetricRequest& request) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetMemoryUtilisationEvents, request);
}


/**
 * @brief AggregationProxy::RequestMarkerEvents
 * @param request
 * @return
 */
QFuture< QVector<MarkerEvent*> > AggregationProxy::RequestMarkerEvents(const MarkerRequest& request) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetMarkerEvents, request);
}


/**
 * @brief AggregationProxy::RequestPortEvents
 * @param request
 * @return
 */
QFuture<QVector<PortEvent*> > AggregationProxy::RequestPortEvents(const PortEventRequest& request) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetPortEvents, request);
}


/**
 * @brief AggregationProxy::RequestNetworkUtilisationEvents
 * @param request
 * @return
 */
QFuture<QVector<NetworkUtilisationEvent*> > AggregationProxy::RequestNetworkUtilisationEvents(const HardwareMetricRequest& request) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetNetworkUtilisationEvents, request);
}


/**
 * @brief AggregationProxy::RequestGPUMetrics
 * @param request
 * @return
 */
QFuture<QVector<AggregationProxy::GPUMetricSample>> AggregationProxy::RequestGPUMetrics(const HardwareMetricRequest& request) const
{
    return QtConcurrent::run(this, &AggregationProxy::GetGPUMetrics, request);
}


/**
 * @brief AggregationProxy::SetServerEndpoint
 * @param endpoint
 * @throws NoRequesterException
 */
void AggregationProxy::SetServerEndpoint(const QString &endpoint)
{
    try {
        requester_.reset(new AggServer::Requester(endpoint.toStdString()));
    } catch (const std::exception& ex) {
        throw NoRequesterException(ex.what());
    }
}


/**
 * @brief AggregationProxy::CheckRequester
 * @throws NoRequesterException
 */
void AggregationProxy::CheckRequester() const
{
    if (!requester_) {
        throw NoRequesterException();
    }
}


/**
 * @brief AggregationProxy::GetExperimentRuns
 * @param experiment_name
 * @throws RequestException
 * @return
 */
QVector<AggServerResponse::ExperimentRun> AggregationProxy::GetExperimentRuns(const QString& experiment_name) const
{
    CheckRequester();

    try {
        QVector<AggServerResponse::ExperimentRun> runs;
        AggServer::ExperimentRunRequest request;
        request.set_experiment_name(experiment_name.toStdString());
        
        const auto& results = requester_->GetExperimentRuns(request);
        for (const auto& exp : results->experiments()) {
            try {
                runs.append(ConstructExperimentRuns(exp));
            } catch (const MalformedProtoException& ex) {
                emit toastNotification(ex.toString(), "chart", Notification::Severity::ERROR);
            }
        }

        return runs;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::ConstructExperimentRuns
 * @param experiment
 * @throws MalformedProtoException
 * @return
 */
QVector<AggServerResponse::ExperimentRun> AggregationProxy::ConstructExperimentRuns(const AggServer::Experiment& experiment) const
{
    const auto& exp_name = ConstructQString(experiment.name());
    if (exp_name.isEmpty()) {
        throw MalformedProtoException("Exception occurred during construction of experiment runs: Experiment name is empty");
    }

    QVector<AggServerResponse::ExperimentRun> runs;
    for (const auto& exp_run : experiment.runs()) {
        AggServerResponse::ExperimentRun run;
        run.experiment_name = exp_name;
        run.experiment_run_id = static_cast<qint32>(exp_run.experiment_run_id());
        run.job_num = exp_run.job_num();
        run.start_time = ConstructQDateTime(exp_run.start_time()).toMSecsSinceEpoch();
        run.end_time = ConstructQDateTime(exp_run.end_time()).toMSecsSinceEpoch();
        runs.append(run);
    }
    return runs;
}


/**
 * @brief AggregationProxy::GetExperimentState
 * @param experiment_run_id
 * @throws RequestException
 * @return
 */
AggServerResponse::ExperimentState AggregationProxy::GetExperimentState(const quint32 experiment_run_id) const
{
    CheckRequester();
    
    try {
        AggServerResponse::ExperimentState state;
        AggServer::ExperimentStateRequest request;
        request.set_experiment_run_id(experiment_run_id);
        
        auto result = requester_->GetExperimentState(request);
        state.last_updated_time = ConstructQDateTime(result->last_updated()).toMSecsSinceEpoch();
        state.end_time = ConstructQDateTime(result->end_time()).toMSecsSinceEpoch();

        for (const auto& n : result->nodes()) {
            auto node = ConvertNode(n);
            state.nodes.push_back(node);
        }
        for (const auto& c : result->components()) {
            state.components.append(ConvertComponent(c));
        }
        for (const auto& w : result->workers()) {
            state.workers.append(ConvertWorker(w));
        }
        for (const auto& p_c : result->port_connections()) {
            state.port_connections.append(ConvertPortConnection(p_c));
        }

        return state;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::GetPortLifecycleEvents
 * @param request
 * @throws RequestException
 * @return
 */
QVector<PortLifecycleEvent*> AggregationProxy::GetPortLifecycleEvents(const PortLifecycleRequest& request) const
{
    CheckRequester();

    try {
        QVector<PortLifecycleEvent*> events;
        AggServer::PortLifecycleRequest agg_request;
        agg_request.set_experiment_run_id(request.experiment_run_id());

        for (const auto& name : request.component_names()) {
            agg_request.add_component_names(name.toStdString());
        }
        for (const auto& id : request.component_instance_ids()) {
            agg_request.add_component_instance_ids(id.toStdString());
        }
        for (const auto& path : request.component_instance_paths()) {
            agg_request.add_component_instance_paths(path.toStdString());
        }
        for (const auto& id : request.port_ids()) {
            agg_request.add_port_ids(id.toStdString());
        }
        for (const auto& path : request.port_paths()) {
            agg_request.add_port_paths(path.toStdString());
        }

        const auto& results = requester_->GetPortLifecycle(agg_request);
        for (const auto& item : results->events()) {
            const auto& port = ConvertPort(item.port());
            const auto& type = ConvertLifeCycleType(item.type());
            const auto& time = ConstructQDateTime(item.time());
            events.append(new PortLifecycleEvent(port, type, time.toMSecsSinceEpoch()));
        }

        return events;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::GetWorkloadEvents
 * @param request
 * @throws RequestException
 * @return
 */
QVector<WorkloadEvent*> AggregationProxy::GetWorkloadEvents(const WorkloadRequest &request) const
{
    CheckRequester();

     try {
        QVector<WorkloadEvent*> events;
        AggServer::WorkloadRequest agg_request;
        agg_request.set_experiment_run_id(request.experiment_run_id());

        for (const auto& name : request.component_names()) {
            agg_request.add_component_names(name.toStdString());
        }
        for (const auto& id : request.component_instance_ids()) {
            agg_request.add_component_instance_ids(id.toStdString());
        }
        for (const auto& path : request.component_instance_paths()) {
            agg_request.add_component_instance_paths(path.toStdString());
        }
        for (const auto& id : request.worker_instance_ids()) {
            agg_request.add_worker_instance_ids(id.toStdString());
        }
        for (const auto& path : request.worker_instance_paths()) {
            agg_request.add_worker_paths(path.toStdString());
        }

        const auto& results = requester_->GetWorkload(agg_request);
        for (const auto& item : results->events()) {
            const auto& worker_instance = ConvertWorkerInstance(item.worker_inst());
            const auto& type = ConvertWorkloadEventType(item.type());
            const auto& workload_id = item.workload_id();
            const auto& time = ConstructQDateTime(item.time()).toMSecsSinceEpoch();
            const auto& function_name = ConstructQString(item.function_name());
            const auto& args = ConstructQString(item.args());
            const auto& log_level = item.log_level();
            events.append(new WorkloadEvent(worker_instance, type, workload_id, time, function_name, args, log_level));
        }

        return events;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::GetCPUUtilisationEvents
 * @param request
 * @throws RequestException
 * @return
 */
QVector<CPUUtilisationEvent*> AggregationProxy::GetCPUUtilisationEvents(const HardwareMetricRequest &request) const
{
    CheckRequester();

    try {
        QVector<CPUUtilisationEvent*> events;
        AggServer::CPUUtilisationRequest agg_request;
        agg_request.set_experiment_run_id(request.experiment_run_id());

        for (const auto& id : request.node_ids()) {
            agg_request.add_node_ids(id.toStdString());
        }
        for (const auto& name : request.node_hostnames()) {
            agg_request.add_node_hostnames(name.toStdString());
        }

        const auto& results = requester_->GetCPUUtilisation(agg_request);
        for (const auto& node : results->nodes()) {
            const auto& host_name = ConstructQString(node.node_info().hostname());
            for (const auto& e : node.events()) {
                const auto& utilisation = e.cpu_utilisation();
                const auto& time = ConstructQDateTime(e.time());
                events.append(new CPUUtilisationEvent(host_name, utilisation, time.toMSecsSinceEpoch()));
            }
        }

        return events;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::GetMemoryUtilisationEvents
 * @param request
 * @throws RequestException
 * @return
 */
QVector<MemoryUtilisationEvent*> AggregationProxy::GetMemoryUtilisationEvents(const HardwareMetricRequest &request) const
{
    CheckRequester();

    try {
        QVector<MemoryUtilisationEvent*> events;
        AggServer::MemoryUtilisationRequest agg_request;
        agg_request.set_experiment_run_id(request.experiment_run_id());

        for (const auto& id : request.node_ids()) {
            agg_request.add_node_ids(id.toStdString());
        }
        for (const auto& name : request.node_hostnames()) {
            agg_request.add_node_hostnames(name.toStdString());
        }

        const auto& results = requester_->GetMemoryUtilisation(agg_request);
        for (const auto& node : results->nodes()) {
            const auto& host_name = ConstructQString(node.node_info().hostname());
            for (const auto& e : node.events()) {
                const auto& utilisation = e.memory_utilisation();
                const auto& time = ConstructQDateTime(e.time());
                events.append(new MemoryUtilisationEvent(host_name, utilisation, time.toMSecsSinceEpoch()));
            }
        }

        return events;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::GetMarkerEvents
 * @param request
 * @throws RequestException
 * @return
 */
QVector<MarkerEvent*> AggregationProxy::GetMarkerEvents(const MarkerRequest &request) const
{
    CheckRequester();

    try {
        QVector<MarkerEvent*> events;
        AggServer::MarkerRequest agg_request;
        agg_request.set_experiment_run_id(request.experiment_run_id());

        for (const auto& name : request.component_names()) {
            agg_request.add_component_names(name.toStdString());
        }
        for (const auto& id : request.component_instance_ids()) {
            agg_request.add_component_instance_ids(id.toStdString());
        }
        for (const auto& path : request.component_instance_paths()) {
            agg_request.add_component_instance_paths(path.toStdString());
        }
        for (const auto& id : request.worker_instance_ids()) {
            agg_request.add_worker_instance_ids(id.toStdString());
        }
        for (const auto& path : request.worker_instance_paths()) {
            agg_request.add_worker_instance_paths(path.toStdString());
        }

        const auto& results = requester_->GetMarkers(agg_request);
        for (const auto& nameSet : results->marker_name_sets()) {
            const auto& name = ConstructQString(nameSet.name());
            for (const auto& idSet : nameSet.marker_id_set()) {
                const auto& id = idSet.id();
                for (const auto& e : idSet.events()) {
                    const auto& compInst = ConvertComponentInstance(e.component_instance());
                    const auto& time = ConstructQDateTime(e.timestamp());
                    events.append(new MarkerEvent(name, id, compInst, time.toMSecsSinceEpoch()));
                }
            }
        }

        return events;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::GetPortEvents
 * @param request
 * @return
 */
QVector<PortEvent*> AggregationProxy::GetPortEvents(const PortEventRequest& request) const
{
    CheckRequester();

    try {
        QVector<PortEvent*> events;
        AggServer::PortEventRequest agg_request;
        agg_request.set_experiment_run_id(request.experiment_run_id());

        for (const auto& name : request.component_names()) {
            agg_request.add_component_names(name.toStdString());
        }
        for (const auto& id : request.component_instance_ids()) {
            agg_request.add_component_instance_ids(id.toStdString());
        }
        for (const auto& path : request.component_instance_paths()) {
            agg_request.add_component_instance_paths(path.toStdString());
        }
        for (const auto& id : request.port_ids()) {
            agg_request.add_port_ids(id.toStdString());
        }
        for (const auto& path : request.port_paths()) {
            agg_request.add_port_paths(path.toStdString());
        }

        const auto& results = requester_->GetPortEvents(agg_request);
        for (const auto& item : results->events()) {
            const auto& port = ConvertPort(item.port());
            const auto& seqNum = item.sequence_num();
            const auto& type = ConvertPortEventType(item.type());
            const auto& message = ConstructQString(item.message());
            const auto& time = ConstructQDateTime(item.time());
            events.append(new PortEvent(port, seqNum, type, message, time.toMSecsSinceEpoch()));
        }

        return events;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::GetNetworkUtilisation
 * @param request
 * @return
 */
QVector<NetworkUtilisationEvent*> AggregationProxy::GetNetworkUtilisationEvents(const HardwareMetricRequest& request) const
{
    CheckRequester();

    try {
        QVector<NetworkUtilisationEvent*> events;
        AggServer::NetworkUtilisationRequest agg_request;
        agg_request.set_experiment_run_id(request.experiment_run_id());

        for (const auto& id : request.node_ids()) {
            agg_request.add_node_ids(id.toStdString());
        }
        for (const auto& name : request.node_hostnames()) {
            agg_request.add_node_hostnames(name.toStdString());
        }

        //auto results = DummyResponseBuilder::getMultiEventsResponse();
        const auto& results = requester_->GetNetworkUtilisation(agg_request);

        // TODO: The NodeNetworkEvents returns a list of InterfaceNetworkEvents per NetworkUtilisationEvent
        //  Ask Jackson - Shouldn't the NetworkUtilisationEvents be grouped by InterfaceNetworkEvent?
        for (const auto& node_network_event : results->node_network_events()) {
            const auto& hostname = ConstructQString(node_network_event.node_info().hostname());
            for (const auto& interface_network_event : node_network_event.events()) {
                const auto& interface_mac_addr = ConstructQString(interface_network_event. interface_mac_addr());
                for (const auto& event : interface_network_event.events()) {
                    const auto& packets_sent = event.packets_sent();
                    const auto& packets_received = event.packets_received();
                    const auto& bytes_sent = event.bytes_sent();
                    const auto& bytes_received = event.bytes_received();
                    const auto& time = ConstructQDateTime(event.time());
                    events.append(new NetworkUtilisationEvent(hostname, interface_mac_addr, packets_sent, packets_received, bytes_sent, bytes_received, time.toMSecsSinceEpoch()));
                }
            }
        }
        
        return events;

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}

QVector<AggregationProxy::GPUMetricSample> AggregationProxy::GetGPUMetrics(const HardwareMetricRequest& request) const
{
    CheckRequester();

    try {

    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::ConvertPort
 * @param proto_port
 * @return
 */
AggServerResponse::Port AggregationProxy::ConvertPort(const AggServer::Port& proto_port)
{
    AggServerResponse::Port port;
    port.kind = ConvertPortKind(proto_port.kind());
    port.name = ConstructQString(proto_port.name());
    port.path = ConstructQString(proto_port.path());
    port.middleware = ConstructQString(proto_port.middleware());
    port.graphml_id = ConstructQString(proto_port.graphml_id());
    return port;
}


/**
 * @brief AggregationProxy::ConvertWorkerInstance
 * @param proto_worker_instance
 * @return
 */
AggServerResponse::WorkerInstance AggregationProxy::ConvertWorkerInstance(const AggServer::WorkerInstance& proto_worker_instance)
{
    AggServerResponse::WorkerInstance worker_instance;
    worker_instance.name = ConstructQString(proto_worker_instance.name());
    worker_instance.path = ConstructQString(proto_worker_instance.path());
    worker_instance.type = ConstructQString(proto_worker_instance.type());
    worker_instance.graphml_id = ConstructQString(proto_worker_instance.graphml_id());
    return worker_instance;
}


/**
 * @brief AggregationProxy::ConvertComponentInstance
 * @param proto_component_instance
 * @return
 */
AggServerResponse::ComponentInstance AggregationProxy::ConvertComponentInstance(const AggServer::ComponentInstance& proto_component_instance)
{
    AggServerResponse::ComponentInstance component_instance;
    component_instance.name = ConstructQString(proto_component_instance.name());
    component_instance.path = ConstructQString(proto_component_instance.path());
    component_instance.graphml_id = ConstructQString(proto_component_instance.graphml_id());
    component_instance.type = ConstructQString(proto_component_instance.type());

    for (const auto& p : proto_component_instance.ports()) {
        component_instance.ports.append(ConvertPort(p));
    }
    for (const auto& w_i : proto_component_instance.worker_instances()) {
        component_instance.worker_instances.append(ConvertWorkerInstance(w_i));
    }

    return component_instance;
}


/**
 * @brief AggregationProxy::ConvertPortConnection
 * @param proto_port_connection
 * @throws std::invalid_argument
 * @return
 */
AggServerResponse::PortConnection AggregationProxy::ConvertPortConnection(const AggServer::PortConnection& proto_port_connection)
{
    AggServerResponse::PortConnection port_connection;
    port_connection.from_port_graphml = ConstructQString(proto_port_connection.from_port_graphml());
    port_connection.to_port_graphml = ConstructQString(proto_port_connection.to_port_graphml());

    switch (proto_port_connection.type()) {
    case AggServer::PortConnection::PUBSUB:
        port_connection.type = AggServerResponse::PortConnection::PUBSUB;
        break;
    case AggServer::PortConnection::REQREP:
        port_connection.type = AggServerResponse::PortConnection::REQREP;
        break;
    default:
        throw std::invalid_argument("AggregationProxy::ConvertPortConnection - Unknown port connection type.");
    }

    return port_connection;
}


/**
 * @brief AggregationProxy::ConvertPortKind
 * @param kind
 * @return
 */
AggServerResponse::Port::Kind AggregationProxy::ConvertPortKind(const AggServer::Port_Kind& kind)
{
    switch (kind) {
    case AggServer::Port::PERIODIC:
        return AggServerResponse::Port::Kind::PERIODIC;
    case AggServer::Port::PUBLISHER:
        return AggServerResponse::Port::Kind::PUBLISHER;
    case AggServer::Port::SUBSCRIBER:
        return AggServerResponse::Port::Kind::SUBSCRIBER;
    case AggServer::Port::REQUESTER:
        return AggServerResponse::Port::Kind::REQUESTER;
    case AggServer::Port::REPLIER:
        return AggServerResponse::Port::Kind::REPLIER;
    default:
        return AggServerResponse::Port::Kind::NO_KIND;
    }
}


/**
 * @brief AggregationProxy::ConvertContainer
 * @param proto_container
 * @return
 */
AggServerResponse::Container AggregationProxy::ConvertContainer(const AggServer::Container& proto_container)
{
    AggServerResponse::Container container;
    container.name = ConstructQString(proto_container.name());
    container.graphml_id = ConstructQString(proto_container.graphml_id());

    for (const auto& c_i : proto_container.component_instances()) {
        container.component_instances.append(ConvertComponentInstance(c_i));
    }

    switch (proto_container.type()) {
    case AggServer::Container::GENERIC:
        container.type = AggServerResponse::Container::ContainerType::GENERIC;
        break;
    case AggServer::Container::DOCKER:
        container.type = AggServerResponse::Container::ContainerType::DOCKER;
        break;
    default:
        break;
    }

    return container;
}


/**
 * @brief AggregationProxy::ConvertNetworkInterface
 * @param proto_network_interface
 * @return
 */
AggServerResponse::NetworkInterface AggregationProxy::ConvertNetworkInterface(const AggServer::NetworkInterface& proto_network_interface)
{
    AggServerResponse::NetworkInterface network_interface;
    network_interface.name = ConstructQString(proto_network_interface.name());
    network_interface.type = ConstructQString(proto_network_interface.type());
    network_interface.description = ConstructQString(proto_network_interface.description());
    network_interface.ipv4 = ConstructQString(proto_network_interface.ipv4());
    network_interface.ipv6 = ConstructQString(proto_network_interface.ipv6());
    network_interface.mac_address = ConstructQString(proto_network_interface.mac_address());
    network_interface.speed = proto_network_interface.speed();
    return network_interface;
}


/**
 * @brief AggregationProxy::ConvertNode
 * @param proto_node
 * @return
 */
AggServerResponse::Node AggregationProxy::ConvertNode(const AggServer::Node& proto_node)
{
    AggServerResponse::Node node;
    node.hostname = ConstructQString(proto_node.hostname());
    node.ip = ConstructQString(proto_node.ip());
    for (const auto& c : proto_node.containers()) {
        node.containers.append(ConvertContainer(c));
    }
    for (const auto& i : proto_node.interfaces()) {
        node.interfaces.append(ConvertNetworkInterface(i));
    }
    return node;
}


/**
 * @brief AggregationProxy::ConvertComponent
 * @param proto_component
 * @return
 */
AggServerResponse::Component AggregationProxy::ConvertComponent(const AggServer::Component& proto_component)
{
    AggServerResponse::Component component;
    component.name = ConstructQString(proto_component.name());
    return component;
}


/**
 * @brief AggregationProxy::ConvertWorker
 * @param proto_worker
 * @return
 */
AggServerResponse::Worker AggregationProxy::ConvertWorker(const AggServer::Worker& proto_worker)
{
    AggServerResponse::Worker worker;
    worker.name = ConstructQString(proto_worker.name());
    return worker;
}


/**
 * @brief AggregationProxy::ConvertLifeCycleType
 * @param type
 * @return
 */
AggServerResponse::LifecycleType AggregationProxy::ConvertLifeCycleType(const AggServer::LifecycleType& type)
{
    switch (type) {
    case AggServer::CONFIGURE:
        return AggServerResponse::LifecycleType::CONFIGURE;
    case AggServer::ACTIVATE:
        return AggServerResponse::LifecycleType::ACTIVATE;
    case AggServer::PASSIVATE:
        return AggServerResponse::LifecycleType::PASSIVATE;
    case AggServer::TERMINATE:
        return AggServerResponse::LifecycleType::TERMINATE;
    default:
        return AggServerResponse::LifecycleType::NO_TYPE;
    }
}


/**
 * @brief AggregationProxy::ConvertWorkloadEventType
 * @param type
 * @return
 */
WorkloadEvent::WorkloadEventType AggregationProxy::ConvertWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType& type)
{
    switch (type) {
    case AggServer::WorkloadEvent::STARTED:
        return WorkloadEvent::WorkloadEventType::STARTED;
    case AggServer::WorkloadEvent::FINISHED:
        return WorkloadEvent::WorkloadEventType::FINISHED;
    case AggServer::WorkloadEvent::MESSAGE:
        return WorkloadEvent::WorkloadEventType::MESSAGE;
    case AggServer::WorkloadEvent::WARNING:
        return WorkloadEvent::WorkloadEventType::WARNING;
    case AggServer::WorkloadEvent::ERROR_EVENT:
        return WorkloadEvent::WorkloadEventType::ERROR_EVENT;
    case AggServer::WorkloadEvent::MARKER:
        return WorkloadEvent::WorkloadEventType::MARKER;
    default:
        return WorkloadEvent::WorkloadEventType::UNKNOWN;
    }
}


/**
 * @brief AggregationProxy::ConvertPortEventType
 * @param type
 * @return
 */
PortEvent::PortEventType AggregationProxy::ConvertPortEventType(const AggServer::PortEvent_PortEventType& type)
{
    switch (type) {
    case AggServer::PortEvent::SENT:
        return PortEvent::PortEventType::SENT;
    case AggServer::PortEvent::RECEIVED:
        return PortEvent::PortEventType::RECEIVED;
    case AggServer::PortEvent::STARTED_FUNC:
        return PortEvent::PortEventType::STARTED_FUNC;
    case AggServer::PortEvent::FINISHED_FUNC:
        return PortEvent::PortEventType::FINISHED_FUNC;
    case AggServer::PortEvent::IGNORED:
        return PortEvent::PortEventType::IGNORED;
    case AggServer::PortEvent::EXCEPTION:
        return PortEvent::PortEventType::EXCEPTION;
    case AggServer::PortEvent::MESSAGE:
        return PortEvent::PortEventType::MESSAGE;
    default:
        return PortEvent::PortEventType::UNKNOWN;
    }
}


/**
 * @brief AggregationProxy::ConstructTimestampFromMS
 * @param milliseconds
 * @return
 */
std::unique_ptr<google::protobuf::Timestamp> AggregationProxy::ConstructTimestampFromMS(qint64 milliseconds)
{
    google::protobuf::Timestamp timestamp = google::protobuf::util::TimeUtil::MillisecondsToTimestamp(milliseconds);
    return std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp(timestamp));
}


/**
 * @brief AggregationProxy::ConstructQDateTime
 * @param time
 * @return
 */
QDateTime AggregationProxy::ConstructQDateTime(const google::protobuf::Timestamp &time)
{
    int64_t mu = google::protobuf::util::TimeUtil::TimestampToMicroseconds(time);
    return QDateTime::fromMSecsSinceEpoch(mu / 1E3, Qt::UTC);
}


/**
 * @brief AggregationProxy::ConstructQString
 * @param str
 * @return
 */
QString AggregationProxy::ConstructQString(const std::string& str)
{
    return QString::fromUtf8(str.c_str());
}
