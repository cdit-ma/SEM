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
        /*
        request_type.add_time_interval();
        request_type.add_port_paths("poop");
        request_type.add_component_instance_paths("");
        request_type.add_component_names("");
        */

        auto results = requester_.GetPortLifecycle(request_type);
        qDebug() << "Result Size#: " << results.get()->events_size();

        QList<PortLifecycleEvent*> events;

        for (auto item : results.get()->events()) {
            // extract port info
            auto port = item.port();
            Port portStruct;
            portStruct.path = QString::fromUtf8(port.path().c_str());
            portStruct.name = QString::fromUtf8(port.name().c_str());
            portStruct.middleware = QString::fromUtf8(port.middleware().c_str());
            //portStruct.component_instance = port->component_instance();

            auto type = getLifeCycleType(item.type());
            auto time = getQDateTime(item.time());

            PortLifecycleEvent* event = new PortLifecycleEvent(portStruct, type, time.toMSecsSinceEpoch());
            events.append(event);
            emit requestResponse(event);

            //qDebug() << "Event: type = " << AggServer::LifecycleType_Name(item.type()).c_str() << " date-time = " << time.toString("MMM d yyyy, hh:mm:ss:zz");
        }

        //emit requestResponse(events);

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
    int64_t msFromSeconds = time.seconds() * 1E3;
    int64_t msFromNanoSeconds = time.nanos() / 1E6;
    return QDateTime::fromMSecsSinceEpoch(msFromSeconds + msFromNanoSeconds, Qt::UTC);
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
/*Port::Kind AggregationProxy::getPortKind(const uint kind)
{

}*/
