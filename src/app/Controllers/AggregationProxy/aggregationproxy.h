#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>
#include <QException>
#include <QVector>
#include <QtConcurrent/QtConcurrentRun>

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
    
    QFuture<QVector<AggServerResponse::ExperimentRun>> RequestExperimentRuns(const QString& experiment_name) const;
    QFuture<AggServerResponse::ExperimentState> RequestExperimentState(const quint32 experiment_run_id) const;

    QFuture<QVector<PortLifecycleEvent*>> RequestPortLifecycleEvents(const PortLifecycleRequest &request) const;
    QFuture<QVector<WorkloadEvent*>> RequestWorkloadEvents(const WorkloadRequest& request) const;
    QFuture<QVector<CPUUtilisationEvent*>> RequestCPUUtilisationEvents(const CPUUtilisationRequest& request) const;
    QFuture<QVector<MemoryUtilisationEvent*>> RequestMemoryUtilisationEvents(const MemoryUtilisationRequest& request) const;
    QFuture<QVector<MarkerEvent*>> RequestMarkerEvents(const MarkerRequest& request) const;

private:
    void SetServerEndpoint(const QString& endpoint);
    void CheckRequester() const;

    QVector<AggServerResponse::ExperimentRun> GetExperimentRuns(const QString& experiment_name) const;
    AggServerResponse::ExperimentState GetExperimentState(const quint32 experiment_run_id) const;

    QVector<PortLifecycleEvent*> GetPortLifecycleEvents(const PortLifecycleRequest &request) const;
    QVector<WorkloadEvent*> GetWorkloadEvents(const WorkloadRequest& request) const;
    QVector<CPUUtilisationEvent*> GetCPUUtilisationEvents(const CPUUtilisationRequest& request) const;
    QVector<MemoryUtilisationEvent*> GetMemoryUtilisationEvents(const MemoryUtilisationRequest& request) const;
    QVector<MarkerEvent*> GetMarkerEvents(const MarkerRequest& request) const;

    // Static Helpers
    static std::unique_ptr<google::protobuf::Timestamp> ConstructTimestampFromMS(qint64 milliseconds);
    static QDateTime ConstructQDateTime(const google::protobuf::Timestamp &time);
    static QString ConstructQString(const std::string &string);
    
    static AggServerResponse::Node ConvertNode(const AggServer::Node& node);
    static AggServerResponse::Component ConvertComponent(const AggServer::Component& component);
    static AggServerResponse::Worker ConvertWorker(const AggServer::Worker& worker);
    static AggServerResponse::Port ConvertPort(const AggServer::Port& port);
    static AggServerResponse::Container ConvertContainer(const AggServer::Container& container);
    static AggServerResponse::WorkerInstance ConvertWorkerInstance(const AggServer::WorkerInstance& workerInstance);
    static AggServerResponse::ComponentInstance ConvertComponentInstance(const AggServer::ComponentInstance& componentInstance);

    static PortLifecycleEvent::PortKind ConvertPortKind(const AggServer::Port_Kind& kind);
    static AggServerResponse::LifecycleType ConvertLifeCycleType(const AggServer::LifecycleType& type);
    static WorkloadEvent::WorkloadEventType ConvertWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType& type);

    std::unique_ptr<AggServer::Requester> requester_;
};

#endif // MEDEA_AGGREGATIONPROXY_H
