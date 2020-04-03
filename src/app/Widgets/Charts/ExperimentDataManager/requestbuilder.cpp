#include "requestbuilder.h"

/**
 * @brief RequestBuilder::build
 */
RequestBuilder RequestBuilder::build()
{
    return RequestBuilder();
}

/**
 * @brief RequestBuilder::buildRequests
 * @param requestKinds
 * @throws std::invalid_argument
 */
void RequestBuilder::buildRequests(const QList<MEDEA::ChartDataKind> &requestKinds)
{
    for (auto kind : requestKinds) {
        switch (kind) {
        case MEDEA::ChartDataKind::PORT_LIFECYCLE:
            portLifecycleRequest_ = std::make_unique<PortLifecycleRequest>(PortLifecycleRequest());
            break;
        case MEDEA::ChartDataKind::WORKLOAD:
            workloadRequest_ = std::make_unique<WorkloadRequest>( WorkloadRequest());
            break;
        case MEDEA::ChartDataKind::CPU_UTILISATION:
            cpuUtilisationRequest_ = std::make_unique<UtilisationRequest>(UtilisationRequest());
            break;
        case MEDEA::ChartDataKind::MEMORY_UTILISATION:
            memoryUtilisationRequest_ = std::make_unique<UtilisationRequest>(UtilisationRequest());
            break;
        case MEDEA::ChartDataKind::MARKER:
            markerRequest_ = std::make_unique<MarkerRequest>(MarkerRequest());
            break;
        case MEDEA::ChartDataKind::PORT_EVENT:
            portEventRequest_ = std::make_unique<PortEventRequest>(PortEventRequest());
            break;
        default:
	        throw std::invalid_argument("RequestBuilder::buildRequests - Unknown chart data kind");
        }
    }
}

/**
 * @brief RequestBuilder::setExperimentRunID
 * @param experiment_run_id
 */
void RequestBuilder::setExperimentRunID(const quint32 experiment_run_id)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setExperimentRunID(experiment_run_id);
    }
    if (workloadRequest_) {
        workloadRequest_->setExperimentRunID(experiment_run_id);
    }
    if (cpuUtilisationRequest_) {
        cpuUtilisationRequest_->setExperimentRunID(experiment_run_id);
    }
    if (memoryUtilisationRequest_) {
        memoryUtilisationRequest_->setExperimentRunID(experiment_run_id);
    }
    if (markerRequest_) {
        markerRequest_->setExperimentRunID(experiment_run_id);
    }
    if (portEventRequest_) {
        portEventRequest_->setExperimentRunID(experiment_run_id);
    }
}

/**
 * @brief RequestBuilder::setTimeInterval
 * @param time_interval
 */
void RequestBuilder::setTimeInterval(const QVector<qint64> &time_interval)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setTimeInterval(time_interval);
    }
    if (workloadRequest_) {
        workloadRequest_->setTimeInterval(time_interval);
    }
    if (cpuUtilisationRequest_) {
        cpuUtilisationRequest_->setTimeInterval(time_interval);
    }
    if (memoryUtilisationRequest_) {
        memoryUtilisationRequest_->setTimeInterval(time_interval);
    }
    if (markerRequest_) {
        markerRequest_->setTimeInterval(time_interval);
    }
    if (portEventRequest_) {
        portEventRequest_->setTimeInterval(time_interval);
    }
}

/**
 * @brief RequestBuilder::setComponentNames
 * @param component_names
 */
void RequestBuilder::setComponentNames(const QVector<QString> &component_names)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setComponentNames(component_names);
    }
    if (workloadRequest_) {
        workloadRequest_->setComponentNames(component_names);
    }
    if (markerRequest_) {
        markerRequest_->setComponentNames(component_names);
    }
    if (portEventRequest_) {
        portEventRequest_->setComponentNames(component_names);
    }
}

/**
 * @brief RequestBuilder::setComponentInstanceIDS
 * @param component_instance_ids
 */
void RequestBuilder::setComponentInstanceIDS(const QVector<QString> &component_instance_ids)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setComponentInstanceIDS(component_instance_ids);
    }
    if (workloadRequest_) {
        workloadRequest_->setComponentInstanceIDS(component_instance_ids);
    }
    if (markerRequest_) {
        markerRequest_->setComponentInstanceIDS(component_instance_ids);
    }
    if (portEventRequest_) {
        portEventRequest_->setComponentInstanceIDS(component_instance_ids);
    }
}

/**
 * @brief RequestBuilder::setComponentInstancePaths
 * @param component_instance_paths
 */
void RequestBuilder::setComponentInstancePaths(const QVector<QString> &component_instance_paths)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setComponentInstancePaths(component_instance_paths);
    }
    if (workloadRequest_) {
        workloadRequest_->setComponentInstancePaths(component_instance_paths);
    }
    if (markerRequest_) {
        markerRequest_->setComponentInstancePaths(component_instance_paths);
    }
    if (portEventRequest_) {
        portEventRequest_->setComponentInstancePaths(component_instance_paths);
    }
}

/**
 * @brief RequestBuilder::setPortIDs
 * @param port_ids
 */
void RequestBuilder::setPortIDs(const QVector<QString> &port_ids)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setPortIDs(port_ids);
    }
    if (portEventRequest_) {
        portEventRequest_->setPortIDs(port_ids);
    }
}

/**
 * @brief RequestBuilder::setPortPaths
 * @param port_paths
 */
void RequestBuilder::setPortPaths(const QVector<QString> &port_paths)
{
    if (portLifecycleRequest_) {
        portLifecycleRequest_->setPortPaths(port_paths);
    }
    if (portEventRequest_) {
        portEventRequest_->setPortPaths(port_paths);
    }
}

/**
 * @brief RequestBuilder::setWorkerInstanceIDs
 * @param worker_instance_ids
 */
void RequestBuilder::setWorkerInstanceIDs(const QVector<QString> &worker_instance_ids)
{
    if (workloadRequest_) {
        workloadRequest_->setWorkerInstanceIDs(worker_instance_ids);
    }
    if (markerRequest_) {
        markerRequest_->setWorkerInstanceIDs(worker_instance_ids);
    }
}

/**
 * @brief RequestBuilder::setWorkerInstancePaths
 * @param worker_paths
 */
void RequestBuilder::setWorkerInstancePaths(const QVector<QString> &worker_instance_paths)
{
    if (workloadRequest_) {
        workloadRequest_->setWorkerInstancePaths(worker_instance_paths);
    }
    if (markerRequest_) {
        markerRequest_->setWorkerInstancePaths(worker_instance_paths);
    }
}

/**
 * @brief RequestBuilder::setNodeIDs
 * @param node_ids
 */
void RequestBuilder::setNodeIDs(const QVector<QString> &node_ids)
{
    if (cpuUtilisationRequest_) {
        cpuUtilisationRequest_->setNodeIDs(node_ids);
    }
    if (memoryUtilisationRequest_) {
        memoryUtilisationRequest_->setNodeIDs(node_ids);
    }
}

/**
 * @brief RequestBuilder::setNodeHostnames
 * @param node_hostnames
 */
void RequestBuilder::setNodeHostnames(const QVector<QString> &node_hostnames)
{
    if (cpuUtilisationRequest_) {
        cpuUtilisationRequest_->setNodeHostnames(node_hostnames);
    }
    if (memoryUtilisationRequest_) {
        memoryUtilisationRequest_->setNodeHostnames(node_hostnames);
    }
}

/**
 * @brief RequestBuilder::getPortLifecycleRequest
 * @throws std::invalid_argument
 * @return
 */
const PortLifecycleRequest& RequestBuilder::getPortLifecycleRequest() const
{
    if (!portLifecycleRequest_) {
        throw std::invalid_argument("There is no built PortLifecycleRequest");
    }
    return *portLifecycleRequest_;
}

/**
 * @brief RequestBuilder::getWorkloadRequest
 * @throws std::invalid_argument
 * @return
 */
const WorkloadRequest& RequestBuilder::getWorkloadRequest() const
{
    if (!workloadRequest_) {
        throw std::invalid_argument("There is no built WorkloadRequest");
    }
    return *workloadRequest_;
}

/**
 * @brief RequestBuilder::getCPUUtilisationRequest
 * @throws std::invalid_argument
 * @return
 */
const UtilisationRequest& RequestBuilder::getCPUUtilisationRequest() const
{
    if (!cpuUtilisationRequest_) {
        throw std::invalid_argument("There is no built CPUUtilisationRequest");
    }
    return *cpuUtilisationRequest_;
}

/**
 * @brief RequestBuilder::getMemoryUtilisationRequest
 * @throws std::invalid_argument
 * @return
 */
const UtilisationRequest& RequestBuilder::getMemoryUtilisationRequest() const
{
    if (!memoryUtilisationRequest_) {
        throw std::invalid_argument("There is no built MemoryUtilisationRequest");
    }
    return *memoryUtilisationRequest_;
}

/**
 * @brief RequestBuilder::getMarkerRequest
 * @throws std::invalid_argument
 * @return
 */
const MarkerRequest& RequestBuilder::getMarkerRequest() const
{
    if (!markerRequest_) {
        throw std::invalid_argument("There is no built MarkerRequest");
    }
    return *markerRequest_;
}

/**
 * @brief RequestBuilder::getPortEventRequest
 * @throws std::invalid_argument
 * @return
 */
const PortEventRequest& RequestBuilder::getPortEventRequest() const
{
    if (!portEventRequest_) {
        throw std::invalid_argument("There is no built PortEventRequest");
    }
    return *portEventRequest_;
}