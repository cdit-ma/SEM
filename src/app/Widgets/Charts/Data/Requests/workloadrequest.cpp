#include "workloadrequest.h"


/**
 * @brief WorkloadRequest::WorkloadRequest
 */
WorkloadRequest::WorkloadRequest() : EventRequest(MEDEA::ChartDataKind::WORKLOAD) {}


/**
 * @brief WorkloadRequest::setWorkerInstanceIDs
 * @param worker_instance_ids
 */
void WorkloadRequest::setWorkerInstanceIDs(const QVector<QString> &worker_instance_ids)
{
    workerInstanceIDs_ = worker_instance_ids;
}


/**
 * @brief WorkloadRequest::setWorkerInstancePaths
 * @param worker_paths
 */
void WorkloadRequest::setWorkerInstancePaths(const QVector<QString> &worker_paths)
{
    workerInstancePaths_ = worker_paths;
}


/**
 * @brief WorkloadRequest::worker_instance_ids
 * @return
 */
const QVector<QString> &WorkloadRequest::worker_instance_ids() const
{
    return workerInstanceIDs_;
}


/**
 * @brief WorkloadRequest::worker_instance_paths
 * @return
 */
const QVector<QString> &WorkloadRequest::worker_instance_paths() const
{
    return workerInstancePaths_;
}
