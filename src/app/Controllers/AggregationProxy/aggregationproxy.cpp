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

        auto start = QDateTime::currentMSecsSinceEpoch();

        // clear previous results in the timeline chart
        emit clearPreviousResults();

        auto results = requester_.GetPortLifecycle(request_type);
        //qDebug() << "Result Size#: " << results.get()->events_size();

        for (auto item : results.get()->events()) {
            auto port = convertPort(item.port());
            auto type = getLifeCycleType(item.type());
            auto time = getQDateTime(item.time());

            PortLifecycleEvent* event = new PortLifecycleEvent(port, type, time.toMSecsSinceEpoch());
            emit requestResponse(event);
        }

        //emit printResults();

        auto finish = QDateTime::currentMSecsSinceEpoch();
        qDebug() << "Construction of portlifecycle widgets and series WITH clearing took: " << finish - start << " ms.";

        /*
        start = QDateTime::currentMSecsSinceEpoch();
        results = requester_.GetPortLifecycle(request_type);
        //qDebug() << "Result Size#: " << results.get()->events_size();

        for (auto item : results.get()->events()) {
            auto port = convertPort(item.port());
            auto type = getLifeCycleType(item.type());
            auto time = getQDateTime(item.time());

            PortLifecycleEvent* event = new PortLifecycleEvent(port, type, time.toMSecsSinceEpoch());
            emit requestResponse(event);
        }

        finish = QDateTime::currentMSecsSinceEpoch();
        qDebug() << "Construction of portlifecycle widgets and series WITHOUT clearing took: " << finish - start << " ms.";
        */

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
 * @brief AggregationProxy::convertPort
 * @param port
 * @return
 */
Port AggregationProxy::convertPort(const AggServer::Port port)
{
    auto compInst = port.component_instance();
    auto comp = compInst.component();
    auto node = compInst.node();

    ComponentInstance compInstStruct;
    compInstStruct.name = getQString(compInst.name());
    compInstStruct.path = getQString(compInst.path());
    compInstStruct.component = Component{getQString(comp.name())};
    compInstStruct.node = Node{getQString(node.hostname()), getQString(node.ip())};

    Port portStruct;
    portStruct.kind = getPortKind(port.kind());
    portStruct.name = getQString(port.name());
    portStruct.path = getQString(port.path());
    portStruct.middleware = getQString(port.middleware());
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

