#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>
#include "../../../app/Widgets/Charts/Data/Events/workloadevent.h"

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
    void receivedWorkloadEvent(WorkloadEvent* event);

private:
    void SendWorkloadRequest(AggServer::WorkloadRequest& request);

    WorkloadEvent::WorkerInstance convertWorkerInstance(const AggServer::WorkerInstance inst);
    WorkloadEvent::WorkloadEventType getWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType type);

    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
