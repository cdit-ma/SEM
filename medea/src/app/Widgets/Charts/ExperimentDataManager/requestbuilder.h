#ifndef REQUESTBUILDER_H
#define REQUESTBUILDER_H

#include "../Data/Requests/portlifecyclerequest.h"
#include "../Data/Requests/workloadrequest.h"
#include "../Data/Requests/hardwaremetricrequest.h"
#include "../Data/Requests/markerrequest.h"
#include "../Data/Requests/porteventrequest.h"

#include <memory>

class RequestBuilder {

protected:
    explicit RequestBuilder() = default;

public:
    static RequestBuilder build();

    void buildRequests(const QList<MEDEA::ChartDataKind> &requestKinds);

    void setExperimentRunID(quint32 experiment_run_id);
    void setTimeInterval(const QVector<qint64>& time_interval);
    void setComponentNames(const QVector<QString>& component_names);
    void setComponentInstanceIDS(const QVector<QString>& component_instance_ids);
    void setComponentInstancePaths(const QVector<QString>& component_instance_paths);
    void setPortIDs(const QVector<QString>& port_ids);
    void setPortPaths(const QVector<QString>& port_paths);
    void setWorkerInstanceIDs(const QVector<QString>& worker_instance_ids);
    void setWorkerInstancePaths(const QVector<QString>& worker_instance_paths);
    void setNodeIDs(const QVector<QString>& node_ids);
    void setNodeHostnames(const QVector<QString>& node_hostnames);

    const PortLifecycleRequest& getPortLifecycleRequest() const;
    const WorkloadRequest& getWorkloadRequest() const;
    const HardwareMetricRequest& getCPUUtilisationRequest() const;
    const HardwareMetricRequest& getMemoryUtilisationRequest() const;
    const MarkerRequest& getMarkerRequest() const;
    const PortEventRequest& getPortEventRequest() const;
    const HardwareMetricRequest& getNetworkUtilisationRequest() const;

private:
    std::unique_ptr<PortLifecycleRequest> portLifecycleRequest_;
    std::unique_ptr<WorkloadRequest> workloadRequest_;
    std::unique_ptr<HardwareMetricRequest> cpuUtilisationRequest_;
    std::unique_ptr<HardwareMetricRequest> memoryUtilisationRequest_;
    std::unique_ptr<MarkerRequest> markerRequest_;
    std::unique_ptr<PortEventRequest> portEventRequest_;
    std::unique_ptr<HardwareMetricRequest> networkUtilisationRequest_;
};

#endif // REQUESTBUILDER_H