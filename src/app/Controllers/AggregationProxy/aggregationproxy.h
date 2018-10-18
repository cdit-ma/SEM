
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

signals:
    void requestResponse(PortLifecycleEvent* event);
    //void requestResponse(QList<PortLifecycleEvent*>& events);

private:
    LifecycleType getLifeCycleType(const AggServer::LifecycleType type);
    //Port::Kind getPortKind(const uint kind);

    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
