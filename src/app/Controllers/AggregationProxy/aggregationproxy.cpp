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
    AggServer::WorkloadRequest request;
    SendWorkloadRequest(request);
}


/**
 * @brief AggregationProxy::RequestRunningExperiments
 * @param fromTimeMS
 * @param toTimeMS
 */
void AggregationProxy::RequestRunningExperiments(qint64 fromTimeMS, qint64 toTimeMS)
{
    AggServer::WorkloadRequest request;
    request.mutable_time_interval()->AddAllocated(constructTimestampFromMS(fromTimeMS).release());
    request.mutable_time_interval()->AddAllocated(constructTimestampFromMS(toTimeMS).release());
    SendWorkloadRequest(request);
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
 * @brief AggregationProxy::SendWorkloadRequest
 * @param request
 */
void AggregationProxy::SendWorkloadRequest(AggServer::WorkloadRequest &request)
{
    auto notification = NotificationManager::manager()->AddNotification("Requesting Workload", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        auto results = requester_.GetWorkload(request);
        emit clearPreviousEvents();

        qDebug() << "--------------------------------------------------------------------------------";
        qDebug() << "[Workload Request] Result size#: " << results.get()->events_size();

        for (auto item : results.get()->events()) {
            auto workerInst = convertWorkerInstance(item.worker_inst());
            auto type = getWorkloadEventType(item.type());
            auto workloadID = item.workload_id();
            auto time = getQDateTime(item.time()).toMSecsSinceEpoch();
            auto funcName = getQString(item.function_name());
            auto args = getQString(item.args());
            auto logLevel = item.log_level();

            //qDebug() << "WORKER ID: " << workerInst.graphml_id;
            //qDebug() << "workload_id: " << QString::number(workloadID);
            //qDebug() << "funcName: " << funcName;

            WorkloadEvent* event = new WorkloadEvent(workerInst, type, workloadID, time, funcName, args, logLevel);
            emit receivedWorkloadEvent(event);
        }

        qDebug() << "--------------------------------------------------------------------------------";
        emit receivedAllEvents();
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
WorkloadEvent::WorkerInstance AggregationProxy::convertWorkerInstance(const AggServer::WorkerInstance inst)
{
    WorkloadEvent::WorkerInstance workerInst;
    workerInst.name = getQString(inst.name());
    workerInst.path = getQString(inst.path());
    workerInst.graphml_id = getQString(inst.grpahml_id());
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
