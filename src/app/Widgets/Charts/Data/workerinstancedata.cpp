#include "workerinstancedata.h"


/**
 * @brief WorkerInstanceData::WorkerInstanceData
 * @param worker_instance
 */
WorkerInstanceData::WorkerInstanceData(const AggServerResponse::WorkerInstance& worker_instance)
    : graphml_id_(worker_instance.graphml_id),
      name_(worker_instance.name),
      path_(worker_instance.path),
      type_(worker_instance.type)
{
    workload_event_series_ = new WorkloadEventSeries(graphml_id_);
    workload_event_series_->setProperty("name", name_);
}


/**
 * @brief WorkerInstanceData::getGraphmlID
 * @return
 */
const QString& WorkerInstanceData::getGraphmlID() const
{
    return graphml_id_;
}


/**
 * @brief WorkerInstanceData::getName
 * @return
 */
const QString& WorkerInstanceData::getName() const
{
    return name_;
}


/**
 * @brief WorkerInstanceData::getPath
 * @return
 */
const QString& WorkerInstanceData::getPath() const
{
    return path_;
}


/**
 * @brief WorkerInstanceData::getType
 * @return
 */
const QString& WorkerInstanceData::getType() const
{
    return type_;
}


/**
 * @brief WorkerInstanceData::addWorkloadEvents
 * @param events
 */
void WorkerInstanceData::addWorkloadEvents(const QVector<WorkloadEvent*>& events)
{
    workload_event_series_->addEvents(events);
}


/**
 * @brief WorkerInstanceData::getWorkloadEventSeries
 * @return
 */
WorkloadEventSeries* WorkerInstanceData::getWorkloadEventSeries() const
{
    return workload_event_series_;
}
