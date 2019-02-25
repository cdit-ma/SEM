#include "aggregationproxy.h"
#include "../SettingsController/settingscontroller.h"

#include <iostream>



/**
 * @brief AggregationProxy::AggregationProxy
 */
AggregationProxy::AggregationProxy()
{
    auto settings = SettingsController::settings();
    connect(settings, &SettingsController::settingChanged, [=](SETTINGS key, QVariant value) {
        if (key == SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT) {
            SetServerEndpoint(value.toString());
        }
    });

    SetServerEndpoint(settings->getSetting(SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT).toString());
}

/**
 * @brief AggregationProxy::SetServerEndpoint
 * @param endpoint
 */
void AggregationProxy::SetServerEndpoint(const QString& endpoint)
{
    requester_.reset(new AggServer::Requester(endpoint.toStdString()));
}


QFuture<QVector<ExperimentRun>> AggregationProxy::RequestExperimentRuns(const QString& experiment_name)
{
    return QtConcurrent::run(this, &AggregationProxy::GetExperimentRuns, experiment_name);
}


QFuture<ExperimentState> AggregationProxy::RequestExperimentState(const quint32 experiment_run_id)
{
    return QtConcurrent::run(this, &AggregationProxy::GetExperimentState, experiment_run_id);
}

QFuture<QVector<PortLifecycleEvent*>> AggregationProxy::RequestPortLifecycleEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& port_ids)
{
    return QtConcurrent::run(this, &AggregationProxy::GetPortLifecycleEvents, experiment_run_id, time_intervals, component_instance_ids, port_ids);
}

QFuture<QVector<WorkloadEvent*>> AggregationProxy::RequestWorkloadEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& worker_ids)
{
    return QtConcurrent::run(this, &AggregationProxy::GetWorkloadEvents, experiment_run_id, time_intervals, component_instance_ids, worker_ids);
}

QFuture<QVector<CPUUtilisationEvent*>> AggregationProxy::RequestCPUUtilisationEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& graphml_ids)
{
    return QtConcurrent::run(this, &AggregationProxy::GetCPUUtilisationEvents, experiment_run_id, time_intervals, graphml_ids);
}

QFuture<QVector<MemoryUtilisationEvent*>> AggregationProxy::RequestMemoryUtilisationEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& graphml_ids)
{
    return QtConcurrent::run(this, &AggregationProxy::GetMemoryUtilisationEvents, experiment_run_id, time_intervals, graphml_ids);
}


/**
 * @brief AggregationProxy::GetExperimentRuns
 * @param experimentName
 * @return
 */
QVector<ExperimentRun> AggregationProxy::GetExperimentRuns(const QString& experiment_name)
{
    CheckRequester();
    
    try {
        QVector<ExperimentRun> runs;
        
        AggServer::ExperimentRunRequest request;
        request.set_experiment_name(experiment_name.toStdString());
        
        auto& results = requester_->GetExperimentRuns(request);
        for (const auto& ex : results->experiments()) {
            const auto& experiment_name = GetQString(ex.name());
            for (auto& ex_run : ex.runs()) {
                ExperimentRun run;
                run.experiment_name = experiment_name;
                run.experiment_run_id = ex_run.experiment_run_id();
                run.job_num = ex_run.job_num();
                run.start_time = GetQDateTime(ex_run.start_time()).toMSecsSinceEpoch();
                run.end_time = GetQDateTime(ex_run.end_time()).toMSecsSinceEpoch();
                runs.append(run);
            }
        }
        return runs;
    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
    
}


/**
 * @brief AggregationProxy::GetExperimentState
 * @param experimentRunID
 * @return
 */
ExperimentState AggregationProxy::GetExperimentState(const quint32 experiment_run_id)
{
    CheckRequester();
    
    try {
        ExperimentState state;

        AggServer::ExperimentStateRequest request;
        request.set_experiment_run_id(experiment_run_id);

        auto& results = requester_->GetExperimentState(request);
        
        state.experiment_run_id = experiment_run_id;
        qDebug() << "[Experiment State Request]";

        for (const auto& n : results->nodes()) {
            state.nodes.append(ConvertNode(n));
        }

        for (const auto& c : results->components()) {
            state.components.append(ConvertComponent(c));
        }

        for (const auto& w : results->workers()) {
            state.workers.append(ConvertWorker(w));
        }
        return state;
    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}

QVector<PortLifecycleEvent*> AggregationProxy::GetPortLifecycleEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& port_ids)
{
    CheckRequester();
    try {
        QVector<PortLifecycleEvent*> events;

        AggServer::PortLifecycleRequest request;
        request.set_experiment_run_id(experiment_run_id);
        
        for (const auto& id : component_instance_ids) {
            request.add_component_instance_ids(id.toStdString());
        }

        for (const auto& id : port_ids) {
            request.add_port_ids(id.toStdString());
        }

        const auto& results = requester_->GetPortLifecycle(request);

        qDebug() << "[PortLifecycle Request] Result size#: " << results->events_size();
        for (const auto& item : results->events()) {
            const auto& port = ConvertPort(item.port());
            const auto& type = ConvertLifeCycleType(item.type());
            const auto& time = GetQDateTime(item.time());
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
 * @return
 */
QVector<WorkloadEvent*> AggregationProxy::GetWorkloadEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& worker_ids)
{
    CheckRequester();

     try {
        QVector<WorkloadEvent*> events;

        AggServer::WorkloadRequest request;
        request.set_experiment_run_id(experiment_run_id);

        for (const auto& id : component_instance_ids) {
            request.add_component_instance_ids(id.toStdString());
        }

        for (const auto& id : worker_ids) {
            request.add_worker_instance_ids(id.toStdString());
        }
        
        const auto& results = requester_->GetWorkload(request);
        qDebug() << "[Workload Request] Result size#: " << results->events_size();
        for (const auto& item : results->events()) {
            const auto& worker_instance = ConvertWorkerInstance(item.worker_inst());
            const auto& type = ConvertWorkloadEventType(item.type());
            const auto& workload_id = item.workload_id();
            const auto& time = GetQDateTime(item.time()).toMSecsSinceEpoch();
            const auto& function_name = GetQString(item.function_name());
            const auto& args = GetQString(item.args());
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
 * @return
 */
QVector<CPUUtilisationEvent*> AggregationProxy::GetCPUUtilisationEvents(
    const quint32 experiment_run_id,
    const QVector<qint64>& time_intervals,
    const QVector<QString>& graphml_ids)
{
    CheckRequester();

    try {
        QVector<CPUUtilisationEvent*> events;

        AggServer::CPUUtilisationRequest request;
        request.set_experiment_run_id(experiment_run_id);

        for (const auto& id : graphml_ids) {
            request.add_node_ids(id.toStdString());
        }
        
        auto results = requester_->GetCPUUtilisation(request);
        qDebug() << "[CPUUtilisation Request] Result size#: " << results->nodes_size();
        for (const auto& node : results->nodes()) {
            const auto& host_name = GetQString(node.node_info().hostname());

            for (const auto& e : node.events()) {
                const auto& utilisation = e.cpu_utilisation();
                const auto& time = GetQDateTime(e.time());
                events.append(new CPUUtilisationEvent(host_name, utilisation, time.toMSecsSinceEpoch()));
            }
        }
        return events;
    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}

QVector<MemoryUtilisationEvent*> AggregationProxy::GetMemoryUtilisationEvents(
    const quint32 experiment_run_id,
    const QVector<qint64>& time_intervals,
    const QVector<QString>& graphml_ids)
{
    CheckRequester();

    try {
        QVector<MemoryUtilisationEvent*> events;

        AggServer::MemoryUtilisationRequest request;
        request.set_experiment_run_id(experiment_run_id);

        for (const auto& id : graphml_ids) {
            request.add_node_ids(id.toStdString());
        }
        
        auto results = requester_->GetMemoryUtilisation(request);
        qDebug() << "[MemoryUtilisation Request] Result size#: " << results->nodes_size();
        for (const auto& node : results->nodes()) {
            const auto& host_name = GetQString(node.node_info().hostname());

            for (const auto& e : node.events()) {
                const auto& utilisation = e.memory_utilisation();
                const auto& time = GetQDateTime(e.time());
                events.append(new MemoryUtilisationEvent(host_name, utilisation, time.toMSecsSinceEpoch()));
            }
        }
        return events;
    } catch (const std::exception& ex) {
        throw RequestException(ex.what());
    }
}


/**
 * @brief AggregationProxy::convertPort
 * @param port
 * @return
 */
Port AggregationProxy::ConvertPort(const AggServer::Port& p)
{
    Port port;
    port.kind = ConvertPortKind(p.kind());
    port.name = GetQString(p.name());
    port.path = GetQString(p.path());
    port.middleware = GetQString(p.middleware());
    port.graphml_id = GetQString(p.graphml_id());
    return port;
}


/**
 * @brief AggregationProxy::convertWorkerInstance
 * @param workerInstance
 * @return
 */
WorkerInstance AggregationProxy::ConvertWorkerInstance(const AggServer::WorkerInstance& w_i)
{
    WorkerInstance worker_instance;
    worker_instance.name = GetQString(w_i.name());
    worker_instance.path = GetQString(w_i.path());
    worker_instance.graphml_id = GetQString(w_i.graphml_id());
    return worker_instance;
}


/**
 * @brief AggregationProxy::convertComponentInstance
 * @param componentInstance
 * @return
 */
ComponentInstance AggregationProxy::ConvertComponentInstance(const AggServer::ComponentInstance& c_i)
{
    ComponentInstance component_instance;
    component_instance.name = GetQString(c_i.name());
    component_instance.path = GetQString(c_i.path());
    component_instance.graphml_id = GetQString(c_i.graphml_id());
    component_instance.type = GetQString(c_i.type());

    for (const auto& p : c_i.ports()) {
        component_instance.ports.append(ConvertPort(p));
    }

    for (const auto& w_i : c_i.worker_instances()) {
        component_instance.worker_instances.append(ConvertWorkerInstance(w_i));
    }

    return component_instance;
}


/**
 * @brief AggregationProxy::convertContainer
 * @param container
 * @return
 */
Container AggregationProxy::ConvertContainer(const AggServer::Container& c)
{
    Container container;
    container.name = GetQString(c.name());
    container.graphml_id = GetQString(c.graphml_id());

    for (const auto& c_i : c.component_instances()) {
        container.component_instances.append(ConvertComponentInstance(c_i));
    }

    switch (c.type()) {
    case AggServer::Container_ContainerType::Container_ContainerType_GENERIC:
        container.type = Container::ContainerType::GENERIC;
        break;
    case AggServer::Container_ContainerType::Container_ContainerType_DOCKER:
        container.type = Container::ContainerType::DOCKER;
        break;
    default:
        break;
    }
    return container;
}

Node AggregationProxy::ConvertNode(const AggServer::Node& n)
{
    Node node;
    node.hostname = GetQString(n.hostname());
    node.ip = GetQString(n.ip());
    for (const auto& c : n.containers()) {
        node.containers.append(ConvertContainer(c));
    }
    return node;
}

Component AggregationProxy::ConvertComponent(const AggServer::Component& c){
    Component component;
    component.name = GetQString(c.name());
    return component;
}


Worker AggregationProxy::ConvertWorker(const AggServer::Worker& w){
    Worker worker;
    worker.name = GetQString(w.name());
    return worker;
}


LifecycleType AggregationProxy::ConvertLifeCycleType(const AggServer::LifecycleType& type)
{
    switch (type) {
    case AggServer::LifecycleType::CONFIGURE:
        return LifecycleType::CONFIGURE;
    case AggServer::LifecycleType::ACTIVATE:
        return LifecycleType::ACTIVATE;
    case AggServer::LifecycleType::PASSIVATE:
        return LifecycleType::PASSIVATE;
    case AggServer::LifecycleType::TERMINATE:
        return LifecycleType::TERMINATE;
    default:
        return LifecycleType::NO_TYPE;
    }
}


Port::Kind AggregationProxy::ConvertPortKind(const AggServer::Port_Kind& kind)
{
    switch (kind) {
    case AggServer::Port_Kind::Port_Kind_PERIODIC:
        return Port::Kind::PERIODIC;
    case AggServer::Port_Kind::Port_Kind_PUBLISHER:
        return Port::Kind::PUBLISHER;
    case AggServer::Port_Kind::Port_Kind_SUBSCRIBER:
        return Port::Kind::SUBSCRIBER;
    case AggServer::Port_Kind::Port_Kind_REQUESTER:
        return Port::Kind::REQUESTER;
    case AggServer::Port_Kind::Port_Kind_REPLIER:
        return Port::Kind::REPLIER;
    default:
        return Port::Kind::NO_KIND;
    }
}


WorkloadEvent::WorkloadEventType AggregationProxy::ConvertWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType& type)
{
    switch (type) {
      case AggServer::WorkloadEvent_WorkloadEventType::WorkloadEvent_WorkloadEventType_STARTED:
          return WorkloadEvent::WorkloadEventType::STARTED;
      case AggServer::WorkloadEvent_WorkloadEventType::WorkloadEvent_WorkloadEventType_FINISHED:
          return WorkloadEvent::WorkloadEventType::FINISHED;
      case AggServer::WorkloadEvent_WorkloadEventType::WorkloadEvent_WorkloadEventType_MESSAGE:
          return WorkloadEvent::WorkloadEventType::MESSAGE;
      case AggServer::WorkloadEvent_WorkloadEventType::WorkloadEvent_WorkloadEventType_WARNING:
          return WorkloadEvent::WorkloadEventType::WARNING;
      case AggServer::WorkloadEvent_WorkloadEventType::WorkloadEvent_WorkloadEventType_ERROR_EVENT:
          return WorkloadEvent::WorkloadEventType::ERROR_EVENT;
      default:
          return WorkloadEvent::WorkloadEventType::UNKNOWN;
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
 * @brief AggregationProxy::GetQDateTime
 * @param time
 * @return
 */
QDateTime AggregationProxy::GetQDateTime(const google::protobuf::Timestamp &time)
{
    int64_t mu = google::protobuf::util::TimeUtil::TimestampToMicroseconds(time);
    return QDateTime::fromMSecsSinceEpoch(mu / 1E3, Qt::UTC);
}


/**
 * @brief AggregationProxy::GetQString
 * @param string
 * @return
 */
QString AggregationProxy::GetQString(const std::string& str)
{
    return QString::fromUtf8(str.c_str());
}

void AggregationProxy::CheckRequester(){
    if(!requester_){
        throw NoRequesterException();
    }
}