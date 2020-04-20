#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>
#include <QException>
#include <QVector>
#include <QtConcurrent/QtConcurrentRun>

#include <memory>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>

#include "../../Widgets/Charts/Data/Events/portlifecycleevent.h"
#include "../../Widgets/Charts/Data/Events/workloadevent.h"
#include "../../Widgets/Charts/Data/Events/cpuutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/memoryutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/markerevent.h"
#include "../../Widgets/Charts/Data/Events/portevent.h"
#include "../../Widgets/Charts/Data/Events/networkutilisationevent.h"
#include "../../Widgets/Charts/ExperimentDataManager/requestbuilder.h"


class NoRequesterException : public QException{
public:
    NoRequesterException() = default;
    NoRequesterException(const QString& error) :
        error_(error) {}

    QString What() const{
        return error_;
    }
    void raise() const { throw *this; }
    NoRequesterException *clone() const { return new NoRequesterException(*this); }
private:
    QString error_;
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
    static AggregationProxy& singleton();

    QFuture<QVector<AggServerResponse::ExperimentRun>> RequestExperimentRuns(const QString& experiment_name) const;
    QFuture<AggServerResponse::ExperimentState> RequestExperimentState(const quint32 experiment_run_id) const;

    QFuture<QVector<PortLifecycleEvent*>> RequestPortLifecycleEvents(const PortLifecycleRequest &request) const;
    QFuture<QVector<WorkloadEvent*>> RequestWorkloadEvents(const WorkloadRequest& request) const;
    QFuture<QVector<CPUUtilisationEvent*>> RequestCPUUtilisationEvents(const CPUUtilisationRequest& request) const;
    QFuture<QVector<MemoryUtilisationEvent*>> RequestMemoryUtilisationEvents(const MemoryUtilisationRequest& request) const;
    QFuture<QVector<MarkerEvent*>> RequestMarkerEvents(const MarkerRequest& request) const;
    QFuture<QVector<PortEvent*>> RequestPortEvents(const PortEventRequest& request) const;
    QFuture<QVector<NetworkUtilisationEvent*>> RequestNetworkUtilisationEvents(const UtilisationRequest& request) const;

private:
    AggregationProxy();

    void SetServerEndpoint(const QString& endpoint);
    void CheckRequester() const;

    QVector<AggServerResponse::ExperimentRun> GetExperimentRuns(const QString& experiment_name) const;
    AggServerResponse::ExperimentState GetExperimentState(const quint32 experiment_run_id) const;

    QVector<PortLifecycleEvent*> GetPortLifecycleEvents(const PortLifecycleRequest &request) const;
    QVector<WorkloadEvent*> GetWorkloadEvents(const WorkloadRequest& request) const;
    QVector<CPUUtilisationEvent*> GetCPUUtilisationEvents(const CPUUtilisationRequest& request) const;
    QVector<MemoryUtilisationEvent*> GetMemoryUtilisationEvents(const MemoryUtilisationRequest& request) const;
    QVector<MarkerEvent*> GetMarkerEvents(const MarkerRequest& request) const;
    QVector<PortEvent*> GetPortEvents(const PortEventRequest& request) const;
    QVector<NetworkUtilisationEvent*> GetNetworkUtilisationEvents(const UtilisationRequest& request) const;

    // Static Helpers
    static std::unique_ptr<google::protobuf::Timestamp> ConstructTimestampFromMS(qint64 milliseconds);
    static QDateTime ConstructQDateTime(const google::protobuf::Timestamp &time);
    static QString ConstructQString(const std::string &string);
    
    static AggServerResponse::Node ConvertNode(const AggServer::Node& proto_node);
    static AggServerResponse::Component ConvertComponent(const AggServer::Component& proto_component);
    static AggServerResponse::Worker ConvertWorker(const AggServer::Worker& proto_worker);
    static AggServerResponse::Port ConvertPort(const AggServer::Port& proto_port);
    static AggServerResponse::Container ConvertContainer(const AggServer::Container& proto_container);
    static AggServerResponse::WorkerInstance ConvertWorkerInstance(const AggServer::WorkerInstance& proto_worker_instance);
    static AggServerResponse::ComponentInstance ConvertComponentInstance(const AggServer::ComponentInstance& proto_component_instance);
    static AggServerResponse::PortConnection ConvertPortConnection(const AggServer::PortConnection& proto_port_connection);

    static AggServerResponse::Port::Kind ConvertPortKind(const AggServer::Port_Kind& kind);
    static AggServerResponse::LifecycleType ConvertLifeCycleType(const AggServer::LifecycleType& type);
    static WorkloadEvent::WorkloadEventType ConvertWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType& type);
    static PortEvent::PortEventType ConvertPortEventType(const AggServer::PortEvent_PortEventType& type);

    static std::unique_ptr<AggregationProxy> proxySingleton_;
    std::unique_ptr<AggServer::Requester> requester_;
};

#endif // MEDEA_AGGREGATIONPROXY_H
