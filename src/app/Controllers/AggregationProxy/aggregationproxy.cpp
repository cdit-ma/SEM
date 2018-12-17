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
    connect(settings, &SettingsController::settingChanged, [=](SETTINGS key, QVariant value){
        if(key == SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT){
            SetServerEndpoint(value.toString());
        }
    });

    SetServerEndpoint(settings->getSetting(SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT).toString());
}

AggregationProxy::~AggregationProxy(){
    if(requester_){
        delete requester_;
    }
}

void AggregationProxy::SetServerEndpoint(QString endpoint){
    if(requester_){
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
    if(!requester_){
        //Got no requester
        return;
    }
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

    emit requstedExperimentRuns(runs);

}


/**
 * @brief AggregationProxy::RequestExperimentState
 * @param experimentRunID
 */
void AggregationProxy::RequestExperimentState(quint32 experimentRunID)
{
    if(!requester_){
        //Got no requester
        return;
    }
    auto notification = NotificationManager::manager()->AddNotification("Request Experiment State", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {

        AggServer::ExperimentStateRequest request;
        request.set_experiment_run_id(experimentRunID);
        setSelectedExperimentRunID(experimentRunID);

        auto& results = requester_->GetExperimentState(request);
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
        //emit setChartUserInputDialogVisible(false);
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
    componentNames_.clear();
    componentNames_.append(componentName);
    setSelectedExperimentRunID(ID);
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

    /*
     *  construct and send requests here
     */

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
    hasSelectedExperimentID_ = true;
}
