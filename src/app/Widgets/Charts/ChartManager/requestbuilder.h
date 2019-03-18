#ifndef REQUESTBUILDER_H
#define REQUESTBUILDER_H

//#include "chartmanager.h"
#include "../Data/Requests/portlifecyclerequest.h"
#include "../Data/Requests/workloadrequest.h"
#include "../Data/Requests/cpuutilisationrequest.h"
#include "../Data/Requests/memoryutilisationrequest.h"
#include "../Data/Requests/markerrequest.h"

class RequestBuilder {

    friend class ChartManager;

protected:
    explicit RequestBuilder();

public:
    RequestBuilder builder();

    void buildRequests(QVector<TIMELINE_DATA_KIND> requestKinds);

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

    const PortLifecycleRequest* getPortLifecycleRequest();
    const WorkloadRequest* getWorkloadRequest();
    const CPUUtilisationRequest* getCPUUtilisationRequest();
    const MemoryUtilisationRequest* getMemoryUtilisationRequest();
    const MarkerRequest* getMarkerRequest();

private:
    PortLifecycleRequest* portLifecycleRequest_ = 0;
    WorkloadRequest* workloadRequest_ = 0;
    CPUUtilisationRequest* cpuUtilisationRequest_ = 0;
    MemoryUtilisationRequest* memoryUtilisationRequest_ = 0;
    MarkerRequest* markerRequest_ = 0;
};

#endif // REQUESTBUILDER_H
