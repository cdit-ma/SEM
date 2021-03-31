#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>
#include <QException>
#include <QVector>
#include <QtConcurrent/QtConcurrentRun>

#include <memory>
#include <utility>

#include <google/protobuf/util/time_util.h>
#include "aggregationrequester.h"

#include "../NotificationManager/notificationenumerations.h"
#include "../../Widgets/Charts/Data/Events/portlifecycleevent.h"
#include "../../Widgets/Charts/Data/Events/workloadevent.h"
#include "../../Widgets/Charts/Data/Events/cpuutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/memoryutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/markerevent.h"
#include "../../Widgets/Charts/Data/Events/portevent.h"
#include "../../Widgets/Charts/Data/Events/networkutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/gpucomputeutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/gpumemoryutilisationevent.h"
#include "../../Widgets/Charts/Data/Events/gputemperatureevent.h"
#include "../../Widgets/Charts/ExperimentDataManager/requestbuilder.h"

class NoRequesterException : public QException {
public:
    NoRequesterException() = default;
    explicit NoRequesterException(QString error)
            : error_(std::move(error.trimmed())) {}
    // TODO: We should make sure we pass the info string to the base std:exception class so that exception::what() works as expected
    QString toString() const{
        return error_;
    }
    void raise() const override { throw *this; }
    NoRequesterException *clone() const override { return new NoRequesterException(*this); }
private:
    QString error_;
};

class RequestException : public QException {
public:
    explicit RequestException(QString error)
            : error_(std::move(error.trimmed())) {}

    QString toString() const{
        return error_;
    }
    void raise() const override { throw *this; }
    RequestException *clone() const override { return new RequestException(*this); }
private:
    QString error_;
};

class MalformedProtoException : public QException {
public:
    explicit MalformedProtoException(QString error)
        : error_(std::move(error.trimmed())) {}

    QString toString() const{
        return error_;
    }
    void raise() const override { throw *this; }
    MalformedProtoException *clone() const override { return new MalformedProtoException(*this); }
private:
    QString error_;
};

/**
 * @brief This class receives the data requests (queries) from the ExperimentManager, gets the response from the
 * AggServer::Requester, and then sends the converted data back to the ExperimentManager
 */
class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    static AggregationProxy& singleton();

    struct GPUMetricSample {
        GPUComputeUtilisationEvent* compute_utilisation = nullptr;
        GPUMemoryUtilisationEvent* memory_utilisation = nullptr;
        GPUTemperatureEvent* temperature = nullptr;
    };

    QFuture<QVector<AggServerResponse::ExperimentRun>> RequestExperimentRuns(const QString& experiment_name) const;
    QFuture<AggServerResponse::ExperimentState> RequestExperimentState(quint32 experiment_run_id) const;

    QFuture<QVector<PortLifecycleEvent*>> RequestPortLifecycleEvents(const PortLifecycleRequest &request) const;
    QFuture<QVector<WorkloadEvent*>> RequestWorkloadEvents(const WorkloadRequest& request) const;
    QFuture<QVector<CPUUtilisationEvent*>> RequestCPUUtilisationEvents(const HardwareMetricRequest& request) const;
    QFuture<QVector<MemoryUtilisationEvent*>> RequestMemoryUtilisationEvents(const HardwareMetricRequest& request) const;
    QFuture<QVector<MarkerEvent*>> RequestMarkerEvents(const MarkerRequest& request) const;
    QFuture<QVector<PortEvent*>> RequestPortEvents(const PortEventRequest& request) const;
    QFuture<QVector<NetworkUtilisationEvent*>> RequestNetworkUtilisationEvents(const HardwareMetricRequest& request) const;
    QFuture<QVector<GPUMetricSample>> RequestGPUMetrics(const HardwareMetricRequest& request) const;

signals:
    void toastNotification(const QString& description, const QString& iconName, Notification::Severity severity) const;

private:
    AggregationProxy();

    void SetServerEndpoint(const QString& endpoint);
    void CheckRequester() const;

    QVector<AggServerResponse::ExperimentRun> GetExperimentRuns(const QString& experiment_name) const;
    AggServerResponse::ExperimentState GetExperimentState(quint32 experiment_run_id) const;

    QVector<PortLifecycleEvent*> GetPortLifecycleEvents(const PortLifecycleRequest& request) const;
    QVector<WorkloadEvent*> GetWorkloadEvents(const WorkloadRequest& request) const;
    QVector<CPUUtilisationEvent*> GetCPUUtilisationEvents(const HardwareMetricRequest& request) const;
    QVector<MemoryUtilisationEvent*> GetMemoryUtilisationEvents(const HardwareMetricRequest& request) const;
    // TODO: Consider grouping the Marker events here (by name_set or id_set)
    QVector<MarkerEvent*> GetMarkerEvents(const MarkerRequest& request) const;
    QVector<PortEvent*> GetPortEvents(const PortEventRequest& request) const;
    QVector<NetworkUtilisationEvent*> GetNetworkUtilisationEvents(const HardwareMetricRequest& request) const;
    QVector<GPUMetricSample> GetGPUMetrics(const HardwareMetricRequest& request) const;

    QVector<AggServerResponse::ExperimentRun> ConstructExperimentRuns(const AggServer::Experiment& experiment) const;

    // Static Helpers
    static std::unique_ptr<google::protobuf::Timestamp> ConstructTimestampFromMS(qint64 milliseconds);
    static QDateTime ConstructQDateTime(const google::protobuf::Timestamp& time);
    static QString ConstructQString(const std::string& string);
    
    static AggServerResponse::Node ConvertNode(const AggServer::Node& proto_node);
    static AggServerResponse::Component ConvertComponent(const AggServer::Component& proto_component);
    static AggServerResponse::Worker ConvertWorker(const AggServer::Worker& proto_worker);
    static AggServerResponse::Port ConvertPort(const AggServer::Port& proto_port);
    static AggServerResponse::Container ConvertContainer(const AggServer::Container& proto_container);
    static AggServerResponse::NetworkInterface ConvertNetworkInterface(const AggServer::NetworkInterface& proto_network_interface);
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
