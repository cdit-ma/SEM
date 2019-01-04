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
        if(key == SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT){
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
    if (!GotRequester())
        return;

    auto notification = NotificationManager::manager()->AddNotification("Request Experiment Runs", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    QList<ExperimentRun> runs;

    try {
        AggServer::ExperimentRunRequest request;
        request.set_experiment_name(experimentName.toStdString());

        auto& results = requester_->GetExperimentRuns(request);

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
        setSelectedExperimentRunID(experimentRunID);

        auto& results = requester_->GetExperimentState(request);
        qDebug() << "[Experiment State] Nodes: " << results->nodes_size();
        qDebug() << "[Experiment State] Components: " << results->components_size();
        qDebug() << "[Experiment State] Workers: " << results->workers_size();
        qDebug() << "--------------------------------------------------------------------------------";

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

        emit requestedExperimentState(hostnames, componentNames, workerNames);
        notification->setSeverity(Notification::Severity::SUCCESS);

    } catch (const std::exception& ex) {
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
}


/**
 * @brief AggregationProxy::RequestEvents
 * @param nodeHostname
 * @param componentName
 * @param workerName
 */
void AggregationProxy::RequestEvents(QString nodeHostname, QString componentName, QString workerName)
{
    qDebug() << "node: " << nodeHostname;
    qDebug() << "component: " << componentName;
    qDebug() << "worker: " << workerName;

    // store request paramenters here
    componentName_ = componentName;
    workerName_ = workerName;

    ReloadRunningExperiments();
}


/**
 * @brief AggregationProxy::ReloadRunningExperiments
 */
void AggregationProxy::ReloadRunningExperiments()
{   
    if (!hasSelectedExperimentID_) {
        emit setChartUserInputDialogVisible(true);
        return;
    }

    emit clearPreviousEvents();

    AggServer::WorkloadRequest workloadRequest;
    workloadRequest.set_experiment_run_id(experimentRunID_);
    //workloadRequest.add
    SendWorkloadRequest(workloadRequest);

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
    hasSelectedExperimentID_ = true;
}


/**
 * @brief AggregationProxy::GotRequester
 * @return
 */
bool AggregationProxy::GotRequester()
{
    if (!requester_) {
        //Got no requester
        NotificationManager::manager()->AddNotification("No Aggregation Requester", "Icons", "buildingPillared", Notification::Severity::ERROR, Notification::Type::APPLICATION, Notification::Category::NONE);
        return false;
    }
    return true;
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
