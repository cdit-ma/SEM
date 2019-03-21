#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>
#include <QException>
#include <QVector>
#include <QtConcurrent/QtConcurrentRun>
#include <QDateTime>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>

#include "../../Widgets/Charts/Data/Events/portlifecycleevent.h"
#include "../../Widgets/Charts/Data/Events/workloadevent.h"
#include "../../Widgets/Charts/Data/Events/cpuutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/memoryutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/markerevent.h"
#include "../../Widgets/Charts/ChartManager/requestbuilder.h"


class NoRequesterException : public QException{
    public:
        void raise() const { throw *this; }
        NoRequesterException *clone() const { return new NoRequesterException(*this); }
};

class RequestException : public QException{
    public:
        RequestException(const QString& error) : 
            error_(error) {}

        QString What() const{
            return error_;
        }
        void raise() const { throw *this; }
        RequestException *clone() const { return new RequestException(*this); }
    private:
        QString error_;
};


class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    AggregationProxy();
    
    QFuture<QVector<ExperimentRun>> RequestExperimentRuns(const QString& experiment_name);
    QFuture<ExperimentState> RequestExperimentState(const quint32 experiment_run_id);

    QFuture<QVector<PortLifecycleEvent*>> RequestPortLifecycleEvents(const PortLifecycleRequest &request);
    QFuture<QVector<WorkloadEvent*>> RequestWorkloadEvents(const WorkloadRequest& request);
    QFuture<QVector<CPUUtilisationEvent*>> RequestCPUUtilisationEvents(const CPUUtilisationRequest& request);
    QFuture<QVector<MemoryUtilisationEvent*>> RequestMemoryUtilisationEvents(const MemoryUtilisationRequest& request);
    QFuture<QVector<MarkerEvent*>> RequestMarkerEvents(const MarkerRequest& request);

private:
    void SetServerEndpoint(const QString& endpoint);
    void CheckRequester();

    QVector<ExperimentRun> GetExperimentRuns(const QString& experiment_name);
    ExperimentState GetExperimentState(const quint32 experiment_run_id);

    QVector<PortLifecycleEvent*> GetPortLifecycleEvents(const PortLifecycleRequest &request);
    QVector<WorkloadEvent*> GetWorkloadEvents(const WorkloadRequest& request);
    QVector<CPUUtilisationEvent*> GetCPUUtilisationEvents(const CPUUtilisationRequest& request);
    QVector<MemoryUtilisationEvent*> GetMemoryUtilisationEvents(const MemoryUtilisationRequest& request);
    QVector<MarkerEvent*> GetMarkerEvents(const MarkerRequest& request);

    // Static Helpers
    static std::unique_ptr<google::protobuf::Timestamp> ConstructTimestampFromMS(qint64 milliseconds);
    static QDateTime ConstructQDateTime(const google::protobuf::Timestamp &time);
    static QString ConstructQString(const std::string &string);
    
    static Node ConvertNode(const AggServer::Node& node);
    static Component ConvertComponent(const AggServer::Component& component);
    static Worker ConvertWorker(const AggServer::Worker& worker);
    static Port ConvertPort(const AggServer::Port& port);
    static Container ConvertContainer(const AggServer::Container& container);
    static WorkerInstance ConvertWorkerInstance(const AggServer::WorkerInstance& workerInstance);
    static ComponentInstance ConvertComponentInstance(const AggServer::ComponentInstance& componentInstance);

    static Port::Kind ConvertPortKind(const AggServer::Port_Kind& kind);
    static LifecycleType ConvertLifeCycleType(const AggServer::LifecycleType& type);
    static WorkloadEvent::WorkloadEventType ConvertWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType& type);


    std::unique_ptr<AggServer::Requester> requester_;
};

#endif // MEDEA_AGGREGATIONPROXY_H
