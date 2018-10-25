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
    auto notification = NotificationManager::manager()->AddNotification("Requesting Port Lifecycle", "Icons", "buildingPillared", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);

    try {
        // setup request
        AggServer::PortLifecycleRequest request_type;
        request_type.add_component_instance_paths("top_level_assembly.1/");
        /*
        request_type.add_time_interval();
        request_type.add_port_paths("poop");
        request_type.add_component_instance_paths("");
        request_type.add_component_names("");
        */

        emit clearPreviousResults();

        auto results = requester_.GetPortLifecycle(request_type);
        qDebug() << "Result Size#: " << results.get()->events_size();

        for (auto item : results.get()->events()) {
            // extract port info
            auto port = convertPort(item.port());
            auto type = getLifeCycleType(item.type());
            auto time = getQDateTime(item.time());

            //qDebug() << "date-time: " << time.toString("MMM d yyyy, hh:mm:ss:zz");
            //qDebug() << "Event: type = " << AggServer::LifecycleType_Name(item.type()).c_str();

            PortLifecycleEvent* event = new PortLifecycleEvent(port, type, time.toMSecsSinceEpoch());
            emit requestResponse(event);
        }

        //emit printResults();
        notification->setSeverity(Notification::Severity::SUCCESS);

    } catch (const std::exception& ex) {
        notification->setSeverity(Notification::Severity::ERROR);
        notification->setDescription(ex.what());
    }
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

    /*
    int64_t msFromSeconds = time.seconds() * 1E3;
    int64_t msFromNanoSeconds = time.nanos() / 1E6;
    return QDateTime::fromMSecsSinceEpoch(msFromSeconds + msFromNanoSeconds, Qt::UTC);
    */
}


/**
 * @brief AggregationProxy::convertPort
 * @param port
 * @return
 */
Port AggregationProxy::convertPort(const AggServer::Port port)
{
    auto compInst = port.component_instance();
    QString compName = QString::fromUtf8(compInst.component().name().c_str());
    QString hostname = QString::fromUtf8(compInst.node().hostname().c_str());
    QString ip = QString::fromUtf8(compInst.node().ip().c_str());

    //Component comp = {QString::fromUtf8(compInst.component().name().c_str())};
    //Node node = {hostname, ip};

    ComponentInstance compInstStruct;
    compInstStruct.name = QString::fromUtf8(compInst.name().c_str());
    compInstStruct.path = QString::fromUtf8(compInst.path().c_str());
    compInstStruct.component = Component{compName};
    compInstStruct.node = Node{hostname, ip};

    Port portStruct;
    portStruct.kind = getPortKind(port.kind());
    portStruct.name = QString::fromUtf8(port.name().c_str());
    portStruct.path = QString::fromUtf8(port.path().c_str());
    portStruct.middleware = QString::fromUtf8(port.middleware().c_str());
    portStruct.component_instance = compInstStruct;
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

