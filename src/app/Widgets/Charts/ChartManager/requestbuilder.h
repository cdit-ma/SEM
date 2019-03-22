#ifndef REQUESTBUILDER_H
#define REQUESTBUILDER_H

//#include "chartmanager.h"
#include "../Data/Requests/portlifecyclerequest.h"
#include "../Data/Requests/workloadrequest.h"
#include "../Data/Requests/cpuutilisationrequest.h"
#include "../Data/Requests/memoryutilisationrequest.h"
#include "../Data/Requests/markerrequest.h"

#include <memory>

class RequestBuilder {

    friend class ChartManager;

protected:
    explicit RequestBuilder();

public:
    static RequestBuilder build();

    void buildRequests(const QList<TIMELINE_DATA_KIND> &requestKinds);

    void setExperimentID(const quint32 experiment_run_id);
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
    const CPUUtilisationRequest& getCPUUtilisationRequest() const;
    const MemoryUtilisationRequest& getMemoryUtilisationRequest() const;
    const MarkerRequest& getMarkerRequest() const;

private:
    std::unique_ptr<PortLifecycleRequest> portLifecycleRequest_;
    std::unique_ptr<WorkloadRequest> workloadRequest_;
    std::unique_ptr<CPUUtilisationRequest> cpuUtilisationRequest_;
    std::unique_ptr<MemoryUtilisationRequest> memoryUtilisationRequest_;
    std::unique_ptr<MarkerRequest> markerRequest_;
};

#endif // REQUESTBUILDER_H
