#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>
#include "../../../app/Widgets/Charts/Data/Events/portlifecycleevent.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    AggregationProxy();

    void RequestRunningExperiments();
    void RequestRunningExperiments(qint64 fromTimeMS, qint64 toTimeMS);

    static std::unique_ptr<google::protobuf::Timestamp> constructTimestampFromMS(qint64 milliseconds);
    static const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    static const QString getQString(const std::string &string);

signals:
    void clearPreviousEvents();
    void receivedAllEvents();
    void receivedPortLifecycleEvent(PortLifecycleEvent* event);

private:    
    void SendPortLifecycleRequest(AggServer::PortLifecycleRequest& request);

    Port convertPort(const AggServer::Port port);
    LifecycleType getLifeCycleType(const AggServer::LifecycleType type);
    Port::Kind getPortKind(const AggServer::Port_Kind kind);

    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
