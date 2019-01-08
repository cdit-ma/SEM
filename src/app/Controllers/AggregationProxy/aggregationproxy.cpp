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
    //qDebug() << "node: " << nodeHostname;
    //qDebug() << "component: " << componentName;
    //qDebug() << "worker: " << workerName;

    // store request paramenters here
    nodeHostname_ = nodeHostname;

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

    AggServer::CPUUtilisationRequest cpuUtilisationRequest;
    cpuUtilisationRequest.set_experiment_run_id(experimentRunID_);
    if (!nodeHostname_.isEmpty())
        cpuUtilisationRequest.add_node_ids(nodeHostname_.toStdString());

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
