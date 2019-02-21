#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>

#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Widgets/Charts/Data/Events/portlifecycleevent.h"
#include "../../Widgets/Charts/Data/Events/workloadevent.h"
#include "../../Widgets/Charts/Data/Events/cpuutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/memoryutilisationevent.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    explicit AggregationProxy();
    ~AggregationProxy();

    void SetServerEndpoint(QString endpoint);
    void SetRequestExperimentRunID(quint32 experimentRunID);

    void RequestExperiments();
    void ReloadExperiments();

    QFuture<QList<ExperimentRun>> RequestExperimentRuns(QString experimentName = "");
    QFuture<ExperimentState> RequestExperimentState(quint32 experimentRunID);
    QFuture<QList<MEDEA::Event*>> RequestAllEvents(quint32 experimentRunID);

    QFuture<QList<MEDEA::Event*>> RequestPortLifecycleEvents(PortLifecycleRequest request);
    QFuture<QList<MEDEA::Event*>> RequestWorkloadEvents(WorkloadRequest request);
    QFuture<QList<MEDEA::Event*>> RequestCPUUtilisationEvents(CPUUtilisationRequest request);
    QFuture<QList<MEDEA::Event*>> RequestMemoryUtilisationEvents(MemoryUtilisationRequest request);

    static std::unique_ptr<google::protobuf::Timestamp> constructTimestampFromMS(qint64 milliseconds);
    static const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    static const QString getQString(const std::string &string);

signals:
    void setChartUserInputDialogVisible(bool visible);

private:
    QList<ExperimentRun> GetExperimentRuns(QString experimentName);
    ExperimentState GetExperimentState(quint32 experimentRunID);
    QList<MEDEA::Event*> GetExperimentEvents(quint32 experimentRunID);

    QList<MEDEA::Event*> GetPortLifecycleEvents(const AggServer::PortLifecycleRequest &request);
    QList<MEDEA::Event*> GetWorkloadEvents(const AggServer::WorkloadRequest &request);
    QList<MEDEA::Event*> GetCPUUtilisationEvents(const AggServer::CPUUtilisationRequest& request);
    QList<MEDEA::Event*> GetMemoryUtilisationEvents(const AggServer::MemoryUtilisationRequest& request);

    bool GotRequester();
    void ResetRequestFilters();

    Port convertPort(const AggServer::Port& port);
    Container convertContainer(AggServer::Container container);
    WorkerInstance convertWorkerInstance(const AggServer::WorkerInstance workerInstance);
    ComponentInstance convertComponentInstance(AggServer::ComponentInstance componentInstance);

    Port::Kind getPortKind(const AggServer::Port_Kind kind);
    LifecycleType getLifeCycleType(const AggServer::LifecycleType type);
    WorkloadEvent::WorkloadEventType getWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType type);

    AggServer::Requester* requester_ = 0;

    bool hasSelectedExperimentID_ = false;
    quint32 experimentRunID_;

    QList<TIMELINE_DATA_KIND> requestEventKinds_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
