#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>

#include "../../Widgets/Charts/Data/Events/protoMessageStructs.h"
#include "../../Widgets/Charts/Data/Events/portlifecycleevent.h"
#include "../../Widgets/Charts/Data/Events/workloadevent.h"
#include "../../Widgets/Charts/Data/Events/cpuutilisationevent.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    explicit AggregationProxy();

    void RequestRunningExperiments();
    void RequestExperimentRuns(QString experimentName = "");
    void RequestExperimentState(quint32 experimentRunID);
    void RequestEvents(quint32 ID, QString componentName);
    //void RequestEvents(QStringList componentNames);
    void ReloadRunningExperiments();

    static std::unique_ptr<google::protobuf::Timestamp> constructTimestampFromMS(qint64 milliseconds);
    static std::string constructStdStringFromQString(QString s);

    static const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    static const QString getQString(const std::string &string);

signals:
    void setChartUserInputDialogVisible(bool visible);
    void requstedExperimentRuns(QList<ExperimentRun> runs);
    void requstedComponentNames(QStringList names);

    void receivedPortLifecycleEvent(PortLifecycleEvent* event);
    void receivedWorkloadEvent(WorkloadEvent* event);
    void receivedCPUUtilisationEvent(CPUUtilisationEvent* event);

    void clearPreviousEvents();
    void receivedAllEvents();

public slots:
    void setSelectedExperimentRunID(quint32 ID);

private:    
    void SendPortLifecycleRequest(AggServer::PortLifecycleRequest& request);
    void SendWorkloadRequest(AggServer::WorkloadRequest& request);
    void SendCPUUtilisationRequest(AggServer::CPUUtilisationRequest& request);

    Port convertPort(const AggServer::Port port);
    LifecycleType getLifeCycleType(const AggServer::LifecycleType type);
    Port::Kind getPortKind(const AggServer::Port_Kind kind);

    WorkloadEvent::WorkerInstance convertWorkerInstance(const AggServer::WorkerInstance inst);
    WorkloadEvent::WorkloadEventType getWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType type);

    quint32 experimentRunID_;
    QStringList componentNames_;

    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
