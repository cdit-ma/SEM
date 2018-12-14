#include "aggregationproxy.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"

#include <iostream>
#include <QtConcurrent>


/**
 * @brief AggregationProxy::AggregationProxy
 */
AggregationProxy::AggregationProxy() :
    requester_("tcp://192.168.111.249:12345")
{
}


/**
 * @brief AggregationProxy::RequestRunningExperiments
 */
void AggregationProxy::RequestRunningExperiments()
{
    emit setChartUserInputDialogVisible(true);
}


/**
 * @brief AggregationProxy::RequestExperimentRuns
 * @param experimentName
 */
void AggregationProxy::RequestExperimentRuns(QString experimentName)
{
    auto notification = NotificationManager::manager()->AddNotification("Request Experiment Runs", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    QList<ExperimentRun> runs;

    try {
        AggServer::ExperimentRunRequest request;
        request.set_experiment_name(experimentName.toStdString());

        auto& results = requester_.GetExperimentRuns(request);

        qDebug() << "--------------------------------------------------------------------------------";
        qDebug() << "Requesting experiment with name: " << experimentName;
        qDebug() << "Results: " << results->experiments_size();
        qDebug() << "--------------------------------------------------------------------------------";

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
        //emit setChartUserInputDialogVisible(false);
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }

    emit requstedExperimentRuns(runs);

}


/**
 * @brief AggregationProxy::RequestExperimentState
 * @param experimentRunID
 */
void AggregationProxy::RequestExperimentState(quint32 experimentRunID)
{
    auto notification = NotificationManager::manager()->AddNotification("Request Experiment State", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {

        AggServer::ExperimentStateRequest request;
        request.set_experiment_run_id(experimentRunID);
        experimentRunID_ = experimentRunID;

        auto& results = requester_.GetExperimentState(request);
        QStringList names;
        qDebug() << "[Experiment State] Results: " << results->components_size();
        qDebug() << "--------------------------------------------------------------------------------";

        for (const auto& component : results->components()) {
            auto name = getQString(component.name());
            names.append(name);
        }

        emit requstedComponentNames(names);
        notification->setSeverity(Notification::Severity::SUCCESS);

    } catch (const std::exception& ex) {
        emit setChartUserInputDialogVisible(false);
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
}


/**
 * @brief AggregationProxy::RequestEvents
 * @param ID
 * @param componentName
 */
void AggregationProxy::RequestEvents(quint32 ID, QString componentName)
{
    experimentRunID_ = ID;
    componentNames_.clear();
    componentNames_.append(componentName);
    ReloadRunningExperiments();
}


/**
 * @brief AggregationProxy::RequestEvents
 * @param componentNames
 */
/*
void AggregationProxy::RequestEvents(QStringList componentNames)
{
    componentNames_ = componentNames;
    ReloadRunningExperiments();
}
*/


/**
 * @brief AggregationProxy::ReloadRunningExperiments
 */
void AggregationProxy::ReloadRunningExperiments()
{
    emit clearPreviousEvents();

    AggServer::PortLifecycleRequest portLifecycleRequest;
    portLifecycleRequest.set_experiment_run_id(experimentRunID_);

    AggServer::WorkloadRequest workloadRequest;
    workloadRequest.set_experiment_run_id(experimentRunID_);

    AggServer::CPUUtilisationRequest cpuUtilisationRequest;
    cpuUtilisationRequest.set_experiment_run_id(experimentRunID_);

    /*for (auto name : componentNames_) {
        portLifecycleRequest.mutable_component_names()->AddAllocated(&constructStdStringFromQString(name));
        workloadRequest.mutable_component_names()->AddAllocated(&constructStdStringFromQString(name));
    }*/

    SendPortLifecycleRequest(portLifecycleRequest);
    SendWorkloadRequest(workloadRequest);
    SendCPUUtilisationRequest(cpuUtilisationRequest);

    emit receivedAllEvents();
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
 * @brief AggregationProxy::constructStdStringFromQString
 * @param s
 * @return
 */
std::string AggregationProxy::constructStdStringFromQString(QString s)
{
    return s.toLocal8Bit().constData();
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
 * @brief AggregationProxy::setSelectedExperimentRunID
 * @param ID
 */
void AggregationProxy::setSelectedExperimentRunID(quint32 ID)
{
    experimentRunID_ = ID;
    ReloadRunningExperiments();
}


/**
 * @brief AggregationProxy::SendPortLifecycleRequest
 * @param request
 */
void AggregationProxy::SendPortLifecycleRequest(AggServer::PortLifecycleRequest &request)
{
    auto notification = NotificationManager::manager()->AddNotification("Requesting Port Lifecycle", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        auto results = requester_.GetPortLifecycle(request);

        qDebug() << "[PortLifecycle Request] Result size#: " << results.get()->events_size();
        qDebug() << "--------------------------------------------------------------------------------";

        for (auto item : results.get()->events()) {
            auto port = convertPort(item.port());
            auto type = getLifeCycleType(item.type());
            auto time = getQDateTime(item.time());
            PortLifecycleEvent* event = new PortLifecycleEvent(port, type, time.toMSecsSinceEpoch());
            emit receivedPortLifecycleEvent(event);
        }

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
    auto notification = NotificationManager::manager()->AddNotification("Requesting Workload", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        auto results = requester_.GetWorkload(request);

        qDebug() << "[Workload Request] Result size#: " << results.get()->events_size();
        qDebug() << "--------------------------------------------------------------------------------";

        for (auto item : results.get()->events()) {
            auto workerInst = convertWorkerInstance(item.worker_inst());
            auto type = getWorkloadEventType(item.type());
            auto workloadID = item.workload_id();
            auto time = getQDateTime(item.time()).toMSecsSinceEpoch();
            auto funcName = getQString(item.function_name());
            auto args = getQString(item.args());
            auto logLevel = item.log_level();
            WorkloadEvent* event = new WorkloadEvent(workerInst, type, workloadID, time, funcName, args, logLevel);
            emit receivedWorkloadEvent(event);
        }

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
    auto notification = NotificationManager::manager()->AddNotification("Requesting CPU Utilisation", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        auto results = requester_.GetCPUUtilisation(request);

        qDebug() << "[CPUUtilisation Request] Result size#: " << results->nodes_size();
        qDebug() << "--------------------------------------------------------------------------------";

        for (const auto& node : results->nodes()) {
            auto hostname = getQString(node.node_info().hostname());
            for (const auto& e : node.events()) {
                auto utilisation = e.cpu_utilisation();
                auto time = getQDateTime(e.time());
                CPUUtilisationEvent* event = new CPUUtilisationEvent(hostname, utilisation, time.toMSecsSinceEpoch());
                emit receivedCPUUtilisationEvent(event);
            }
        }

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
 * @brief AggregationProxy::convertWorkerInstance
 * @param inst
 * @return
 */
WorkloadEvent::WorkerInstance AggregationProxy::convertWorkerInstance(const AggServer::WorkerInstance inst)
{
    WorkloadEvent::WorkerInstance workerInst;
    workerInst.name = getQString(inst.name());
    workerInst.path = getQString(inst.path());
    workerInst.graphml_id = getQString(inst.graphml_id());
    return workerInst;
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
