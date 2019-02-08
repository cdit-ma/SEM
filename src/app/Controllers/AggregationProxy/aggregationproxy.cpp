#include "aggregationproxy.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"

#include "../SettingsController/settingscontroller.h"

#include <iostream>
#include <QtConcurrent>


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
 * @brief AggregationProxy::SetRequestEventKinds
 * @param kinds
 */
void AggregationProxy::SetRequestEventKinds(QList<TIMELINE_DATA_KIND> kinds)
{
    for (auto kind : kinds) {
        qDebug() << "Data kind: " << GET_TIMELINE_DATA_KIND_STRING(kind);
    }
    qDebug() << "-----------------------------------------------------------";
    if (!kinds.isEmpty()) {
        requestEventKinds_ = kinds;
    }
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

    emit clearPreviousEvents();
    SendRequests();
    emit receivedAllEvents();
}


/**
 * @brief AggregationProxy::RequestExperimentRuns
 * @param experimentName
 */
void AggregationProxy::RequestExperimentRuns(QString experimentName)
{
    if (!GotRequester())
        return;

    auto notification = NotificationManager::manager()->AddNotification("Request Experiment Runs", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    QList<ExperimentRun> runs;

    try {
        AggServer::ExperimentRunRequest request;
        request.set_experiment_name(experimentName.toStdString());

        auto& results = requester_->GetExperimentRuns(request);

        /*qDebug() << "--------------------------------------------------------------------------------";
        qDebug() << "Requesting experiment with name: " << experimentName;
        qDebug() << "Results: " << results->experiments_size();
        qDebug() << "--------------------------------------------------------------------------------";*/

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

        notification->setSeverity(Notification::Severity::SUCCESS);

    } catch (const std::exception& ex) {
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }

    emit requestedExperimentRuns(runs);
}


/**
 * @brief AggregationProxy::RequestExperimentState
 * @param experimentRunID
 */
void AggregationProxy::RequestExperimentState(quint32 experimentRunID)
{
    if (!GotRequester())
        return;

    auto notification = NotificationManager::manager()->AddNotification("Request Experiment State", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        AggServer::ExperimentStateRequest request;
        request.set_experiment_run_id(experimentRunID);

        auto& results = requester_->GetExperimentState(request);
        qDebug() << "[Experiment State] Nodes: " << results->nodes_size();
        qDebug() << "[Experiment State] Components: " << results->components_size();
        qDebug() << "[Experiment State] Workers: " << results->workers_size();
        qDebug() << "-----------------------------------------------------------";
        QStringList hostnames, componentNames, workerNames;
        for (const auto& node : results->nodes()) {
            auto name = getQString(node.hostname());
            hostnames.append(name);
        }
        for (const auto& component : results->components()) {
            auto name = getQString(component.name());
            componentNames.append(name);
        }
        for (const auto& worker : results->workers()) {
            auto name = getQString(worker.name());
            workerNames.append(name);
        }

        //emit requestedExperimentState(hostnames, componentNames, workerNames);
        notification->setSeverity(Notification::Severity::SUCCESS);

        //qDebug() << getQDateTime(results->end_time()).toMSecsSinceEpoch();

    } catch (const std::exception& ex) {
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
}


/**
 * @brief AggregationProxy::RequestAllEvents
 */
void AggregationProxy::RequestAllEvents()
{
    SendRequests();
    emit receivedAllEvents();
}


/**
 * @brief AggregationProxy::RequestPortLifecycleEvents
 * @param request
 */
void AggregationProxy::RequestPortLifecycleEvents(PortLifecycleRequest request)
{
    if (!hasSelectedExperimentID_)
        return;

    AggServer::PortLifecycleRequest portLifecycleRequest;
    portLifecycleRequest.set_experiment_run_id(experimentRunID_);
    qDebug() << "Request PortLifecycleEvents";

    for (auto compName : request.component_names) {
        qDebug() << "comp: " << compName;
        portLifecycleRequest.add_component_names(compName.toStdString());
    }
    for (auto compInstPath : request.component_instance_paths) {
        qDebug() << "compInstPath: " << compInstPath;
        portLifecycleRequest.add_component_instance_paths(compInstPath.toStdString());
    }
    for (auto portPath : request.port_paths) {
        qDebug() << "portPath: " << portPath;
        portLifecycleRequest.add_port_paths(portPath.toStdString());
    }

    SendPortLifecycleRequest(portLifecycleRequest);
}


/**
 * @brief AggregationProxy::RequestWorkloadEvents
 * @param request
 */
void AggregationProxy::RequestWorkloadEvents(WorkloadRequest request)
{
    if (!hasSelectedExperimentID_)
        return;

    AggServer::WorkloadRequest workloadRequest;
    workloadRequest.set_experiment_run_id(experimentRunID_);
    qDebug() << "Request WorkloadEvents";

    for (auto compName : request.component_names) {
        qDebug() << "comp: " << compName;
        workloadRequest.add_component_names(compName.toStdString());
    }
    for (auto compInstPath : request.component_instance_paths) {
        qDebug() << "compInstPath: " << compInstPath;
        workloadRequest.add_component_instance_paths(compInstPath.toStdString());
    }
    for (auto workerPath : request.worker_paths) {
        qDebug() << "workerPath: " << workerPath;
        workloadRequest.add_worker_paths(workerPath.toStdString());
    }

    SendWorkloadRequest(workloadRequest);
}


/**
 * @brief AggregationProxy::RequestCPUUtilisationEvents
 * @param request
 */
void AggregationProxy::RequestCPUUtilisationEvents(CPUUtilisationRequest request)
{
    AggServer::CPUUtilisationRequest cpuUtilisationRequest;
    cpuUtilisationRequest.set_experiment_run_id(experimentRunID_);
    qDebug() << "Request CPUUtilisationEvents";

    for (auto nodeID : request.node_ids) {
        qDebug() << "nodeID: " << nodeID;
        cpuUtilisationRequest.add_node_ids(nodeID.toStdString());
    }

    SendCPUUtilisationRequest(cpuUtilisationRequest);
}


/**
 * @brief AggregationProxy::RequestMemoryUtilisationEvents
 * @param request
 */
void AggregationProxy::RequestMemoryUtilisationEvents(MemoryUtilisationRequest request)
{
    AggServer::MemoryUtilisationRequest memoryUtilisationRequest;
    memoryUtilisationRequest.set_experiment_run_id(experimentRunID_);
    qDebug() << "Request MemoryUtilisationEvents";

    for (auto nodeID : request.node_ids) {
        qDebug() << "nodeID: " << nodeID;
        memoryUtilisationRequest.add_node_ids(nodeID.toStdString());
    }

    SendMemoryUtilisationRequest(memoryUtilisationRequest);
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
    requestEventKinds_ = GET_TIMELINE_DATA_KINDS();
    hasSelectedExperimentID_ = false;
    experimentRunID_ = -1;
    componentName_ = "";
    workerName_ = "";
    nodeHostname_ = "";
}


/**
 * @brief AggregationProxy::SendRequests
 */
void AggregationProxy::SendRequests()
{
    if (!hasSelectedExperimentID_)
        return;

    for (auto kind : requestEventKinds_) {
        switch (kind) {
        case TIMELINE_DATA_KIND::PORT_LIFECYCLE: {
            AggServer::PortLifecycleRequest request;
            request.set_experiment_run_id(experimentRunID_);
            SendPortLifecycleRequest(request);
            break;
        }
        case TIMELINE_DATA_KIND::WORKLOAD: {
            AggServer::WorkloadRequest request;
            request.set_experiment_run_id(experimentRunID_);
            SendWorkloadRequest(request);
            break;
        }
        case TIMELINE_DATA_KIND::CPU_UTILISATION: {
            AggServer::CPUUtilisationRequest request;
            request.set_experiment_run_id(experimentRunID_);
            SendCPUUtilisationRequest(request);
            break;
        }
        case TIMELINE_DATA_KIND::MEMORY_UTILISATION: {
            AggServer::MemoryUtilisationRequest request;
            request.set_experiment_run_id(experimentRunID_);
            SendMemoryUtilisationRequest(request);
            break;
        }
        default:
            break;
        }
    }
}


/**
 * @brief AggregationProxy::SendPortLifecycleRequest
 * @param request
 */
void AggregationProxy::SendPortLifecycleRequest(AggServer::PortLifecycleRequest &request)
{
    if (!GotRequester())
        return;

    auto notification = NotificationManager::manager()->AddNotification("Requesting Port Lifecycle", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        auto results = requester_->GetPortLifecycle(request);
        QList<MEDEA::Event*> events;

        qDebug() << "[PortLifecycle Request] Result size#: " << results.get()->events_size();
        qDebug() << "-----------------------------------------------------------";
        for (auto item : results.get()->events()) {
            auto port = convertPort(item.port());
            auto type = getLifeCycleType(item.type());
            auto time = getQDateTime(item.time());
            PortLifecycleEvent* event = new PortLifecycleEvent(port, type, time.toMSecsSinceEpoch());
            events.append(event);
            //emit receivedPortLifecycleEvent(event);
        }

        //emit receivedPortLifecycleEvents(events);
        emit receivedEvents(experimentRunID_, events);
        notification->setSeverity(Notification::Severity::SUCCESS);

    } catch (const std::exception& ex) {
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
}


/**
 * @brief AggregationProxy::SendWorkloadRequest
 * @param request
 */
void AggregationProxy::SendWorkloadRequest(AggServer::WorkloadRequest &request)
{
    if (!GotRequester())
        return;

    auto notification = NotificationManager::manager()->AddNotification("Requesting Workload", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        auto results = requester_->GetWorkload(request);
        QList<MEDEA::Event*> events;

        qDebug() << "[Workload Request] Result size#: " << results->events_size();
        qDebug() << "-----------------------------------------------------------";
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
            //emit receivedWorkloadEvent(event);
        }

        //emit receivedWorkloadEvents(events);
        emit receivedEvents(experimentRunID_, events);
        notification->setSeverity(Notification::Severity::SUCCESS);

    } catch (const std::exception& ex) {
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
}


/**
 * @brief AggregationProxy::SendCPUUtilisationRequest
 * @param request
 */
void AggregationProxy::SendCPUUtilisationRequest(AggServer::CPUUtilisationRequest &request)
{
    if (!GotRequester())
        return;

    auto notification = NotificationManager::manager()->AddNotification("Requesting CPU Utilisation", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        auto results = requester_->GetCPUUtilisation(request);
        QList<MEDEA::Event*> events;

        qDebug() << "[CPUUtilisation Request] Result size#: " << results->nodes_size();
        qDebug() << "-----------------------------------------------------------";
        for (const auto& node : results->nodes()) {
            auto hostname = getQString(node.node_info().hostname());
            //qDebug() << "host: " << hostname << " - #" << node.events_size();
            for (const auto& e : node.events()) {
                auto utilisation = e.cpu_utilisation();
                auto time = getQDateTime(e.time());
                CPUUtilisationEvent* event = new CPUUtilisationEvent(hostname, utilisation, time.toMSecsSinceEpoch());
                events.append(event);
                //emit receivedCPUUtilisationEvent(event);
            }
        }

        //emit receivedCPUUtilisationEvents(events);
        emit receivedEvents(experimentRunID_, events);
        notification->setSeverity(Notification::Severity::SUCCESS);

    } catch (const std::exception& ex) {
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
}


/**
 * @brief AggregationProxy::SendMemoryUtilisationRequest
 * @param request
 */
void AggregationProxy::SendMemoryUtilisationRequest(AggServer::MemoryUtilisationRequest& request)
{
    if (!GotRequester())
         return;

     auto notification = NotificationManager::manager()->AddNotification("Requesting Memory Utilisation", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

     try {
         auto results = requester_->GetMemoryUtilisation(request);
         QList<MEDEA::Event*> events;

         qDebug() << "[MemoryUtilisation Request] Result size#: " << results->nodes_size();
         qDebug() << "-----------------------------------------------------------";
         for (const auto& node : results->nodes()) {
             auto hostname = getQString(node.node_info().hostname());
             for (const auto& e : node.events()) {
                 auto utilisation = e.memory_utilisation();
                 auto time = getQDateTime(e.time());
                 MemoryUtilisationEvent* event = new MemoryUtilisationEvent(hostname, utilisation, time.toMSecsSinceEpoch());
                 events.append(event);
                 //emit receivedMemoryUtilisationEvent(event);
             }
         }

         //emit receivedMemoryUtilisationEvents(events);
         emit receivedEvents(experimentRunID_, events);
         notification->setSeverity(Notification::Severity::SUCCESS);

     } catch (const std::exception& ex) {
         notification->setSeverity(Notification::Severity::ERROR);
         notification->setDescription(ex.what());
     }
}


/**
 * @brief AggregationProxy::convertPort
 * @param port
 * @return
 */
Port AggregationProxy::convertPort(const AggServer::Port port)
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
 * @param inst
 * @return
 */
WorkerInstance AggregationProxy::convertWorkerInstance(const AggServer::WorkerInstance inst)
{
    WorkerInstance workerInst;
    workerInst.name = getQString(inst.name());
    workerInst.path = getQString(inst.path());
    workerInst.graphml_id = getQString(inst.graphml_id());
    return workerInst;
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
