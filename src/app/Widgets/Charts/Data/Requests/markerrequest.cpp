#include "markerrequest.h"


/**
 * @brief MarkerRequest::MarkerRequest
 */
MarkerRequest::MarkerRequest() : EventRequest(MEDEA::ChartDataKind::MARKER) {}


/**
 * @brief MarkerRequest::setWorkerInstanceIDs
 * @param worker_instance_ids
 */
void MarkerRequest::setWorkerInstanceIDs(const QVector<QString> &worker_instance_ids)
{
    workerInstanceIDs_ = worker_instance_ids;
}


/**
 * @brief MarkerRequest::setWorkerInstancePaths
 * @param worker_paths
 */
void MarkerRequest::setWorkerInstancePaths(const QVector<QString> &worker_paths)
{
    workerInstancePaths_ = worker_paths;
}


/**
 * @brief MarkerRequest::worker_instance_ids
 * @return
 */
const QVector<QString> &MarkerRequest::worker_instance_ids() const
{
    return workerInstanceIDs_;
}


/**
 * @brief MarkerRequest::worker_instance_paths
 * @return
 */
const QVector<QString> &MarkerRequest::worker_instance_paths() const
{
    return workerInstancePaths_;
}
