
#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>
#include "../../../app/Widgets/Charts/Timeline/Data/portlifecycleevent.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    AggregationProxy();

    void RequestRunningExperiments();

    const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    const QString getQString(const std::string &string);

signals:
    void receivedPortLifecycleEvent(Port port, LifecycleType type, qint64 time);
    void requestResponse(PortLifecycleEvent* event);
    void clearPreviousResults();
    void printResults();

private:
    Port convertPort(const AggServer::Port port);
    LifecycleType getLifeCycleType(const AggServer::LifecycleType type);
    Port::Kind getPortKind(const AggServer::Port_Kind kind);

    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
