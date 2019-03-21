#include "requestbuilder.h"
#include <QDebug>

/**
 * @brief RequestBuilder::RequestBuilder
 */
RequestBuilder::RequestBuilder() {}


/**
 * @brief RequestBuilder::buildRequests
 * @param requestKinds
 */
RequestBuilder RequestBuilder::builder()
{
    return RequestBuilder();
}


/**
 * @brief RequestBuilder::buildRequests
 * @param requestKinds
 */
void RequestBuilder::buildRequests(QVector<TIMELINE_DATA_KIND> requestKinds)
{
    for (auto kind : requestKinds) {
        switch (kind) {
        case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
            portLifecycleRequest_ = new PortLifecycleRequest();
            break;
        case TIMELINE_DATA_KIND::WORKLOAD:
            workloadRequest_ = new WorkloadRequest();
            break;
        case TIMELINE_DATA_KIND::CPU_UTILISATION:
            cpuUtilisationRequest_ = new CPUUtilisationRequest();
            break;
        case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
            memoryUtilisationRequest_ = new MemoryUtilisationRequest();
            break;
        case TIMELINE_DATA_KIND::MARKER:
            markerRequest_ = new MarkerRequest();
            break;
        default:
            break;
        }
    }
}


/**
 * @brief RequestBuilder::setExperimentID
 * @param experiment_run_id
 */
void RequestBuilder::setExperimentID(const quint32 experiment_run_id)
{
    if (portLifecycleRequest_)
        portLifecycleRequest_->setExperimentID(experiment_run_id);

    if (workloadRequest_)
        workloadRequest_->setExperimentID(experiment_run_id);

    if (cpuUtilisationRequest_)
        cpuUtilisationRequest_->setExperimentID(experiment_run_id);

    if (memoryUtilisationRequest_)
        memoryUtilisationRequest_->setExperimentID(experiment_run_id);

    if (markerRequest_)
        markerRequest_->setExperimentID(experiment_run_id);
}


/**
 * @brief RequestBuilder::setTimeInterval
 * @param time_interval
 */
void RequestBuilder::setTimeInterval(const QVector<qint64> &time_interval)
{
    if (portLifecycleRequest_)
        portLifecycleRequest_->setTimeInterval(time_interval);

    if (workloadRequest_)
        workloadRequest_->setTimeInterval(time_interval);

    if (cpuUtilisationRequest_)
        cpuUtilisationRequest_->setTimeInterval(time_interval);

    if (memoryUtilisationRequest_)
        memoryUtilisationRequest_->setTimeInterval(time_interval);

    if (markerRequest_)
        markerRequest_->setTimeInterval(time_interval);
}


/**
 * @brief RequestBuilder::setComponentNames
 * @param component_names
 */
void RequestBuilder::setComponentNames(const QVector<QString> &component_names)
{
    for (auto name : component_names) {
        qDebug() << "Comp name: " << name;
    }

    if (portLifecycleRequest_)
        portLifecycleRequest_->setComponentNames(component_names);

    if (workloadRequest_)
        workloadRequest_->setComponentNames(component_names);

    if (markerRequest_)
        markerRequest_->setComponentNames(component_names);
}


/**
 * @brief RequestBuilder::setComponentInstanceIDS
 * @param component_instance_ids
 */
void RequestBuilder::setComponentInstanceIDS(const QVector<QString> &component_instance_ids)
{
    if (portLifecycleRequest_)
        portLifecycleRequest_->setComponentInstanceIDS(component_instance_ids);

    if (workloadRequest_)
        workloadRequest_->setComponentInstanceIDS(component_instance_ids);

    if (markerRequest_)
        markerRequest_->setComponentInstanceIDS(component_instance_ids);
}


/**
 * @brief RequestBuilder::setComponentInstancePaths
 * @param component_instance_paths
 */
void RequestBuilder::setComponentInstancePaths(const QVector<QString> &component_instance_paths)
{
    for (auto path : component_instance_paths) {
        qDebug() << "CompInst path: " << path;
    }

    if (portLifecycleRequest_)
        portLifecycleRequest_->setComponentInstancePaths(component_instance_paths);

    if (workloadRequest_)
        workloadRequest_->setComponentInstancePaths(component_instance_paths);

    if (markerRequest_)
        markerRequest_->setComponentInstancePaths(component_instance_paths);
}


/**
 * @brief RequestBuilder::setPortIDs
 * @param port_ids
 */
void RequestBuilder::setPortIDs(const QVector<QString> &port_ids)
{
    if (portLifecycleRequest_)
        portLifecycleRequest_->setPortIDs(port_ids);
}


/**
 * @brief RequestBuilder::setPortPaths
 * @param port_paths
 */
void RequestBuilder::setPortPaths(const QVector<QString> &port_paths)
{
    for (auto path : port_paths) {
        qDebug() << "Port path: " << path;
    }

    if (portLifecycleRequest_)
        portLifecycleRequest_->setPortPaths(port_paths);
}


/**
 * @brief RequestBuilder::setWorkerInstanceIDs
 * @param worker_instance_ids
 */
void RequestBuilder::setWorkerInstanceIDs(const QVector<QString> &worker_instance_ids)
{
    if (workloadRequest_)
        workloadRequest_->setWorkerInstanceIDs(worker_instance_ids);

    if (markerRequest_)
        markerRequest_->setWorkerInstanceIDs(worker_instance_ids);
}


/**
 * @brief RequestBuilder::setWorkerInstancePaths
 * @param worker_paths
 */
void RequestBuilder::setWorkerInstancePaths(const QVector<QString> &worker_instance_paths)
{
    for (auto path : worker_instance_paths) {
        qDebug() << "WorkerInst path: " << path;
    }

    if (workloadRequest_)
        workloadRequest_->setWorkerInstancePaths(worker_instance_paths);

    if (markerRequest_)
        markerRequest_->setWorkerInstancePaths(worker_instance_paths);
}


/**
 * @brief RequestBuilder::setNodeIDs
 * @param node_ids
 */
void RequestBuilder::setNodeIDs(const QVector<QString> &node_ids)
{
    if (cpuUtilisationRequest_)
        cpuUtilisationRequest_->setNodeIDs(node_ids);

    if (memoryUtilisationRequest_)
        memoryUtilisationRequest_->setNodeIDs(node_ids);
}


/**
 * @brief RequestBuilder::setNodeHostnames
 * @param node_hostnames
 */
void RequestBuilder::setNodeHostnames(const QVector<QString> &node_hostnames)
{
    for (auto hostname : node_hostnames) {
        qDebug() << "Hostname: " << hostname;
    }

    if (cpuUtilisationRequest_)
        cpuUtilisationRequest_->setNodeHostnames(node_hostnames);

    if (memoryUtilisationRequest_)
        memoryUtilisationRequest_->setNodeHostnames(node_hostnames);
}


/**
 * @brief RequestBuilder::getPortLifecycleRequest
 * @return
 */
const PortLifecycleRequest *RequestBuilder::getPortLifecycleRequest() const
{
    return portLifecycleRequest_;
}


/**
 * @brief RequestBuilder::getWorkloadRequest
 * @return
 */
const WorkloadRequest *RequestBuilder::getWorkloadRequest() const
{
    return workloadRequest_;
}


/**
 * @brief RequestBuilder::getCPUUtilisationRequest
 * @return
 */
const CPUUtilisationRequest *RequestBuilder::getCPUUtilisationRequest() const
{
    return cpuUtilisationRequest_;
}


/**
 * @brief RequestBuilder::getMemoryUtilisationRequest
 * @return
 */
const MemoryUtilisationRequest *RequestBuilder::getMemoryUtilisationRequest() const
{
    return memoryUtilisationRequest_;
}


/**
 * @brief RequestBuilder::getMarkerRequest
 * @return
 */
const MarkerRequest *RequestBuilder::getMarkerRequest() const
{
    return markerRequest_;
}
