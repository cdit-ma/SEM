#include "aggregationproxy.h"
#include "../SettingsController/settingscontroller.h"

#include <iostream>
#include <QtConcurrent>


/**
 * @brief AggregationProxy::AggregationProxy
 */
AggregationProxy::AggregationProxy()
{

    qRegisterMetaType<QList<ExperimentRun>>();

    auto settings = SettingsController::settings();
    connect(settings, &SettingsController::settingChanged, [=](SETTINGS key, QVariant value) {
        if (key == SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT) {
            SetServerEndpoint(value.toString());
        }
    });

    SetServerEndpoint(settings->getSetting(SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT).toString());
}


/**
 * @brief AggregationProxy::~AggregationProxy
 */
AggregationProxy::~AggregationProxy()
{
    if (requester_) {
        delete requester_;
    }
}


/**
 * @brief AggregationProxy::SetServerEndpoint
 * @param endpoint
 */
void AggregationProxy::SetServerEndpoint(QString endpoint)
{
    if (requester_) {
        delete requester_;
    }
    requester_ = new AggServer::Requester(endpoint.toStdString());
}


/**
 * @brief AggregationProxy::SetRequestExperimentRunID
 * @param experimentRunID
 */
void AggregationProxy::SetRequestExperimentRunID(quint32 experimentRunID)
{
    experimentRunID_ = experimentRunID;
    hasSelectedExperimentID_ = true;
}


/**
 * @brief AggregationProxy::RequestRunningExperiments
 */
void AggregationProxy::RequestExperiments()
{
    ResetRequestFilters();
    emit setChartUserInputDialogVisible(true);
}


/**
 * @brief AggregationProxy::ReloadRunningExperiments
 */
void AggregationProxy::ReloadExperiments()
{
    if (!hasSelectedExperimentID_) {
        emit setChartUserInputDialogVisible(true);
        return;
    }

    // TODO:: Reload the data being displayed in the chart
}


/**
 * @brief AggregationProxy::RequestExperimentRuns
 * @param experimentName
 * @return
 */
QFuture<QList<ExperimentRun>> AggregationProxy::RequestExperimentRuns(QString experimentName)
{
    return QtConcurrent::run(this, &AggregationProxy::GetExperimentRuns, experimentName);
}


/**
 * @brief AggregationProxy::RequestExperimentState
 * @param experimentRunID
 * @return
 */
QFuture<ExperimentState> AggregationProxy::RequestExperimentState(quint32 experimentRunID)
{
    return QtConcurrent::run(this, &AggregationProxy::GetExperimentState, experimentRunID);
}


/**
 * @brief AggregationProxy::RequestAllEvents
 * @param experimentRunID
 * @return
 */
QFuture<QList<MEDEA::Event*>> AggregationProxy::RequestAllEvents(quint32 experimentRunID)
{
    return QtConcurrent::run(this, &AggregationProxy::GetExperimentEvents, experimentRunID);
}


/**
 * @brief AggregationProxy::RequestPortLifecycleEvents
 * @param request
 * @return
 */
QFuture<QList<MEDEA::Event*>> AggregationProxy::RequestPortLifecycleEvents(PortLifecycleRequest request)
{
    if (!hasSelectedExperimentID_)
        return QFuture<QList<MEDEA::Event*>>();

    AggServer::PortLifecycleRequest portLifecycleRequest;
    portLifecycleRequest.set_experiment_run_id(experimentRunID_);
    for (auto compName : request.component_names) {
        portLifecycleRequest.add_component_names(compName.toStdString());
    }
    for (auto compInstPath : request.component_instance_paths) {
        portLifecycleRequest.add_component_instance_paths(compInstPath.toStdString());
    }
    for (auto portPath : request.paths) {
        portLifecycleRequest.add_port_paths(portPath.toStdString());
    }
    for (auto portID : request.graphml_ids) {
        portLifecycleRequest.add_port_ids(portID.toStdString());
        qDebug() << "port ID: " << portID;
    }

    return QtConcurrent::run(this, &AggregationProxy::GetPortLifecycleEvents, portLifecycleRequest);
}


/**
 * @brief AggregationProxy::RequestWorkloadEvents
 * @param request
 * @return
 */
QFuture<QList<MEDEA::Event*>> AggregationProxy::RequestWorkloadEvents(WorkloadRequest request)
{
    if (!hasSelectedExperimentID_)
        return QFuture<QList<MEDEA::Event*>>();

    AggServer::WorkloadRequest workloadRequest;
    workloadRequest.set_experiment_run_id(experimentRunID_);
    for (auto compName : request.component_names) {
        workloadRequest.add_component_names(compName.toStdString());
    }
    for (auto compInstPath : request.component_instance_paths) {
        workloadRequest.add_component_instance_paths(compInstPath.toStdString());
    }
    for (auto workerPath : request.paths) {
        workloadRequest.add_worker_paths(workerPath.toStdString());
    }

    return QtConcurrent::run(this, &AggregationProxy::GetWorkloadEvents, workloadRequest);
}

QFuture<QList<MEDEA::Event*>> AggregationProxy::RequestCPUUtilisationEvents(CPUUtilisationRequest request)
{
    if (!hasSelectedExperimentID_)
        return QFuture<QList<MEDEA::Event*>>();

    AggServer::CPUUtilisationRequest cpuUtilisationRequest;
    cpuUtilisationRequest.set_experiment_run_id(experimentRunID_);
    for (auto hostname : request.node_hostnames) {
        cpuUtilisationRequest.add_node_hostnames(hostname.toStdString());
    }

    return QtConcurrent::run(this, &AggregationProxy::GetCPUUtilisationEvents, cpuUtilisationRequest);
}


/**
 * @brief AggregationProxy::RequestMemoryUtilisationEvents
 * @param request
 * @return
 */
QFuture<QList<MEDEA::Event*>> AggregationProxy::RequestMemoryUtilisationEvents(MemoryUtilisationRequest request)
{
    if (!hasSelectedExperimentID_)
        return QFuture<QList<MEDEA::Event*>>();

    AggServer::MemoryUtilisationRequest memoryUtilisationRequest;
    memoryUtilisationRequest.set_experiment_run_id(experimentRunID_);
    for (auto hostname : request.node_hostnames) {
        memoryUtilisationRequest.add_node_hostnames(hostname.toStdString());
    }

    return QtConcurrent::run(this, &AggregationProxy::GetMemoryUtilisationEvents, memoryUtilisationRequest);
}


/**
 * @brief AggregationProxy::constructTimestampFromMS
 * @param milliseconds
 * @return
 */
std::unique_ptr<google::protobuf::Timestamp> AggregationProxy::constructTimestampFromMS(qint64 milliseconds)
{
    google::protobuf::Timestamp timestamp = google::protobuf::util::TimeUtil::MillisecondsToTimestamp(milliseconds);
    return std::unique_ptr<google::protobuf::Timestamp>(new google::protobuf::Timestamp(timestamp));
}


/**
 * @brief AggregationProxy::getQDateTime
 * @param time
 * @return
 */
const QDateTime AggregationProxy::getQDateTime(const google::protobuf::Timestamp &time)
{
    int64_t mu = google::protobuf::util::TimeUtil::TimestampToMicroseconds(time);
    return QDateTime::fromMSecsSinceEpoch(mu / 1E3, Qt::UTC);
}


/**
 * @brief AggregationProxy::getQString
 * @param string
 * @return
 */
const QString AggregationProxy::getQString(const std::string &string)
{
    return QString::fromUtf8(string.c_str());
}


/**
 * @brief AggregationProxy::GetExperimentRuns
 * @param experimentName
 * @return
 */
QList<ExperimentRun> AggregationProxy::GetExperimentRuns(QString experimentName)
{
    QList<ExperimentRun> runs;

    if (!GotRequester())
        return runs;

    try {
        AggServer::ExperimentRunRequest request;
        request.set_experiment_name(experimentName.toStdString());
        auto& results = requester_->GetExperimentRuns(request);
        for (const auto& ex : results->experiments()) {
            auto experiment_name = getQString(ex.name());
            for (auto& exRun : ex.runs()) {
                ExperimentRun run;
                run.experiment_name = experiment_name;
                run.experiment_run_id = exRun.experiment_run_id();
                run.job_num = exRun.job_num();
                run.start_time = getQDateTime(exRun.start_time()).toMSecsSinceEpoch();
                run.end_time = getQDateTime(exRun.end_time()).toMSecsSinceEpoch();
                runs.append(run);
            }
        }
    } catch (const std::exception& ex) {
        throw std::runtime_error(ex.what());
    }

    return runs;
}


/**
 * @brief AggregationProxy::GetExperimentState
 * @param experimentRunID
 * @return
 */
ExperimentState AggregationProxy::GetExperimentState(quint32 experimentRunID)
{
    ExperimentState state;

    if (!GotRequester())
        return state;

    try {
        AggServer::ExperimentStateRequest request;
        request.set_experiment_run_id(experimentRunID);

        auto& results = requester_->GetExperimentState(request);
        state.experiment_run_id = experimentRunID;
        qDebug() << "[Experiment State Request]";

        for (const auto& n : results->nodes()) {
            Node node;
            node.hostname = getQString(n.hostname());
            node.ip = getQString(n.ip());
            for (auto c : n.containers()) {
                node.containers.append(convertContainer(c));
            }
            state.nodes.append(node);
        }

        for (const auto& c : results->components()) {
            Component component;
            component.name = getQString(c.name());
            state.components.append(component);
        }

        for (const auto& w : results->workers()) {
            Worker worker;
            worker.name = getQString(w.name());
            state.workers.append(worker);
        }

    } catch (const std::exception& ex) {
        throw std::runtime_error(ex.what());
    }

    return state;
}


/**
 * @brief AggregationProxy::GetExperimentEvents
 * @param experimentRunID
 * @return
 */
QList<MEDEA::Event*> AggregationProxy::GetExperimentEvents(quint32 experimentRunID)
{
    QList<MEDEA::Event*> events;

    if (!GotRequester())
        return events;

    AggServer::PortLifecycleRequest portLifecycleRequest;
    portLifecycleRequest.set_experiment_run_id(experimentRunID);
    events.append(GetPortLifecycleEvents(portLifecycleRequest));

    AggServer::WorkloadRequest workloadRequest;
    workloadRequest.set_experiment_run_id(experimentRunID);
    events.append(GetWorkloadEvents(workloadRequest));

    AggServer::CPUUtilisationRequest cpuUtilisationRequest;
    cpuUtilisationRequest.set_experiment_run_id(experimentRunID);
    events.append(GetCPUUtilisationEvents(cpuUtilisationRequest));

    AggServer::MemoryUtilisationRequest memoryUtilisationRequest;
    memoryUtilisationRequest.set_experiment_run_id(experimentRunID);
    events.append(GetMemoryUtilisationEvents(memoryUtilisationRequest));

    return events;
}


/**
 * @brief AggregationProxy::GetPortLifecycleEvents
 * @param request
 * @return
 */
QList<MEDEA::Event*> AggregationProxy::GetPortLifecycleEvents(const AggServer::PortLifecycleRequest &request)
{
    QList<MEDEA::Event*> events;

    if (!GotRequester())
        return events;

    try {
        const auto results = requester_->GetPortLifecycle(request);
        qDebug() << "[PortLifecycle Request] Result size#: " << results.get()->events_size();
        for (auto item : results.get()->events()) {
            auto port = convertPort(item.port());
            auto type = getLifeCycleType(item.type());
            auto time = getQDateTime(item.time());
            PortLifecycleEvent* event = new PortLifecycleEvent(port, type, time.toMSecsSinceEpoch());
            events.append(event);
        }
    } catch (const std::exception& ex) {
        throw std::runtime_error(ex.what());
    }

    return events;
}


/**
 * @brief AggregationProxy::GetWorkloadEvents
 * @param request
 * @return
 */
QList<MEDEA::Event*> AggregationProxy::GetWorkloadEvents(const AggServer::WorkloadRequest &request)
{
    QList<MEDEA::Event*> events;

    if (!GotRequester())
        return events;

    try {
        auto results = requester_->GetWorkload(request);
        qDebug() << "[Workload Request] Result size#: " << results->events_size();
        for (auto item : results->events()) {
            auto workerInst = convertWorkerInstance(item.worker_inst());
            auto type = getWorkloadEventType(item.type());
            auto workloadID = item.workload_id();
            auto time = getQDateTime(item.time()).toMSecsSinceEpoch();
            auto funcName = getQString(item.function_name());
            auto args = getQString(item.args());
            auto logLevel = item.log_level();
            WorkloadEvent* event = new WorkloadEvent(workerInst, type, workloadID, time, funcName, args, logLevel);
            events.append(event);
        }
    } catch (const std::exception& ex) {
        throw std::runtime_error(ex.what());
    }

    return events;
}


/**
 * @brief AggregationProxy::GetCPUUtilisationEvents
 * @param request
 * @return
 */
QList<MEDEA::Event*> AggregationProxy::GetCPUUtilisationEvents(const AggServer::CPUUtilisationRequest &request)
{
    QList<MEDEA::Event*> events;

    if (!GotRequester())
        return events;

    try {
        auto results = requester_->GetCPUUtilisation(request);
        qDebug() << "[CPUUtilisation Request] Result size#: " << results->nodes_size();
        for (const auto& node : results->nodes()) {
            auto hostname = getQString(node.node_info().hostname());
            for (const auto& e : node.events()) {
                auto utilisation = e.cpu_utilisation();
                auto time = getQDateTime(e.time());
                CPUUtilisationEvent* event = new CPUUtilisationEvent(hostname, utilisation, time.toMSecsSinceEpoch());
                events.append(event);
            }
        }
    } catch (const std::exception& ex) {
        throw std::runtime_error(ex.what());
    }

    return events;
}


/**
 * @brief AggregationProxy::GetMemoryUtilisationEvents
 * @param request
 * @return
 */
QList<MEDEA::Event *> AggregationProxy::GetMemoryUtilisationEvents(const AggServer::MemoryUtilisationRequest &request)
{
    QList<MEDEA::Event*> events;

    if (!GotRequester())
        return events;

     try {
         auto results = requester_->GetMemoryUtilisation(request);
         qDebug() << "[MemoryUtilisation Request] Result size#: " << results->nodes_size();
         for (const auto& node : results->nodes()) {
             auto hostname = getQString(node.node_info().hostname());
             for (const auto& e : node.events()) {
                 auto utilisation = e.memory_utilisation();
                 auto time = getQDateTime(e.time());
                 MemoryUtilisationEvent* event = new MemoryUtilisationEvent(hostname, utilisation, time.toMSecsSinceEpoch());
                 events.append(event);
             }
         }
     } catch (const std::exception& ex) {
        throw std::runtime_error(ex.what());
     }

    return events;
}


/**
 * @brief AggregationProxy::GotRequester
 * @return
 */
bool AggregationProxy::GotRequester()
{
    if (!requester_) {
        NotificationManager::manager()->AddNotification("No Aggregation Requester", "Icons", "buildingPillared", Notification::Severity::ERROR, Notification::Type::APPLICATION, Notification::Category::NONE);
        return false;
    }
    return true;
}


/**
 * @brief AggregationProxy::ResetRequestFilters
 */
void AggregationProxy::ResetRequestFilters()
{
    experimentRunID_ = -1;
    hasSelectedExperimentID_ = false;
}


/**
 * @brief AggregationProxy::convertPort
 * @param port
 * @return
 */
Port AggregationProxy::convertPort(const AggServer::Port& port)
{
    Port portStruct;
    portStruct.kind = getPortKind(port.kind());
    portStruct.name = getQString(port.name());
    portStruct.path = getQString(port.path());
    portStruct.middleware = getQString(port.middleware());
    portStruct.graphml_id = getQString(port.graphml_id());
    return portStruct;
}


/**
 * @brief AggregationProxy::convertWorkerInstance
 * @param workerInstance
 * @return
 */
WorkerInstance AggregationProxy::convertWorkerInstance(const AggServer::WorkerInstance workerInstance)
{
    WorkerInstance workerInstStruct;
    workerInstStruct.name = getQString(workerInstance.name());
    workerInstStruct.path = getQString(workerInstance.path());
    workerInstStruct.graphml_id = getQString(workerInstance.graphml_id());
    return workerInstStruct;
}


/**
 * @brief AggregationProxy::convertComponentInstance
 * @param componentInstance
 * @return
 */
ComponentInstance AggregationProxy::convertComponentInstance(AggServer::ComponentInstance componentInstance)
{
    ComponentInstance compInstStruct;
    compInstStruct.name = getQString(componentInstance.name());
    compInstStruct.path = getQString(componentInstance.path());
    compInstStruct.graphml_id = getQString(componentInstance.graphml_id());
    compInstStruct.type = getQString(componentInstance.type());

    for (auto port : componentInstance.ports()) {
        compInstStruct.ports.append(convertPort(port));
    }

    for (auto workerInst : componentInstance.worker_instances()) {
        compInstStruct.worker_instances.append(convertWorkerInstance(workerInst));
    }

    return compInstStruct;
}


/**
 * @brief AggregationProxy::convertContainer
 * @param container
 * @return
 */
Container AggregationProxy::convertContainer(AggServer::Container container)
{
    Container containerStruct;
    containerStruct.name = getQString(container.name());
    containerStruct.graphml_id = getQString(container.graphml_id());

    for (auto compInst : container.component_instances()) {
        containerStruct.component_instances.append(convertComponentInstance(compInst));
    }

    switch (container.type()) {
    case AggServer::Container_ContainerType::Container_ContainerType_GENERIC:
        containerStruct.type = Container::ContainerType::GENERIC;
        break;
    case AggServer::Container_ContainerType::Container_ContainerType_DOCKER:
        containerStruct.type = Container::ContainerType::DOCKER;
        break;
    }

    return containerStruct;
}


/**
 * @brief AggregationProxy::getLifeCycleType
 * @param type
 * @return
 */
LifecycleType AggregationProxy::getLifeCycleType(const AggServer::LifecycleType type)
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


/**
 * @brief AggregationProxy::getPortKind
 * @param kind
 * @return
 */
Port::Kind AggregationProxy::getPortKind(const AggServer::Port_Kind kind)
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


/**
 * @brief AggregationProxy::getWorkloadEventType
 * @param type
 * @return
 */
WorkloadEvent::WorkloadEventType AggregationProxy::getWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType type)
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
