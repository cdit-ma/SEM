#include "aggregationproxy.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"

#include <iostream>
#include <QtConcurrent>


AggregationProxy::AggregationProxy() :
    requester_("tcp://192.168.111.249:12345")
{
}


void AggregationProxy::RequestRunningExperiments()
{
    AggServer::PortLifecycleRequest request;
    SendPortLifecycleRequest(request);
}


/**
 * @brief AggregationProxy::RequestRunningExperiments
 * @param fromTimeMS
 * @param toTimeMS
 */
void AggregationProxy::RequestRunningExperiments(qint64 fromTimeMS, qint64 toTimeMS)
{
    AggServer::PortLifecycleRequest request;
    request.mutable_time_interval()->AddAllocated(constructTimestampFromMS(fromTimeMS).release());
    request.mutable_time_interval()->AddAllocated(constructTimestampFromMS(toTimeMS).release());
    SendPortLifecycleRequest(request);
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
 * @brief AggregationProxy::SendPortLifecycleRequest
 * @param request
 */
void AggregationProxy::SendPortLifecycleRequest(AggServer::PortLifecycleRequest &request)
{
    auto notification = NotificationManager::manager()->AddNotification("Requesting Port Lifecycle", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        auto start = QDateTime::currentMSecsSinceEpoch();
        auto results = requester_.GetPortLifecycle(request);

        qDebug() << "--------------------------------------------------------------------------------";
        qDebug() << "Requested PortLifecycleEvents . . . ";
        qDebug() << "Result Size#: " << results.get()->events_size();

        emit clearPreviousResults();

        for (auto item : results.get()->events()) {
            auto port = convertPort(item.port());
            auto type = getLifeCycleType(item.type());
            auto time = getQDateTime(item.time());

            //emit receivedPortLifecycleEvent(port, type, time.toMSecsSinceEpoch());
            //continue;

            PortLifecycleEvent* event = new PortLifecycleEvent(port, type, time.toMSecsSinceEpoch());
            emit requestResponse(event);
        }

        auto finish = QDateTime::currentMSecsSinceEpoch();
        qDebug() << "Construction of portlifecycle widgets and series WITH clearing took: " << finish - start << " ms.";
        qDebug() << "--------------------------------------------------------------------------------";

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

