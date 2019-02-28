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
#include "../../Widgets/Charts/Data/Events/protoMessageStructs.h"


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

    QFuture<QVector<PortLifecycleEvent*>> RequestPortLifecycleEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& port_ids);

    QFuture<QVector<WorkloadEvent*>> RequestWorkloadEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& worker_ids);
    
    QFuture<QVector<CPUUtilisationEvent*>> RequestCPUUtilisationEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& graphml_ids);
    
    QFuture<QVector<MemoryUtilisationEvent*>> RequestMemoryUtilisationEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& graphml_ids);
    
private:
    void SetServerEndpoint(const QString& endpoint);

    QVector<ExperimentRun> GetExperimentRuns(const QString& experiment_name);
    ExperimentState GetExperimentState(const quint32 experiment_run_id);

    QVector<PortLifecycleEvent*> GetPortLifecycleEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& port_ids);

    QVector<WorkloadEvent*> GetWorkloadEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& worker_ids);
    
    QVector<CPUUtilisationEvent*> GetCPUUtilisationEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& graphml_ids);
    
    QVector<MemoryUtilisationEvent*> GetMemoryUtilisationEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& graphml_ids);

    void CheckRequester();
    std::unique_ptr<AggServer::Requester> requester_;


    //Static Helpers
    static std::unique_ptr<google::protobuf::Timestamp> ConstructTimestampFromMS(qint64 milliseconds);
    static QDateTime GetQDateTime(const google::protobuf::Timestamp &time);
    static QString GetQString(const std::string &string);
    
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
};

#endif // MEDEA_AGGREGATIONPROXY_H
