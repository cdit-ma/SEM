#include "workerinstancedata.h"
#include "componentinstancedata.h"
#include "../ExperimentDataManager/experimentdatamanager.h"

/**
 * @brief WorkerInstanceData::WorkerInstanceData
 * @param exp_run_id
 * @param comp_inst
 * @param worker_instance
 * @param parent
 */
WorkerInstanceData::WorkerInstanceData(quint32 exp_run_id, const ComponentInstanceData& comp_inst, const AggServerResponse::WorkerInstance& worker_instance, QObject* parent)
    : QObject(parent),
      experiment_run_id_(exp_run_id),
      last_updated_time_(0),
      graphml_id_(worker_instance.graphml_id),
      name_(worker_instance.name),
      path_(worker_instance.path),
      type_(worker_instance.type)
{
    // Setup request
    workload_request_.setExperimentRunID(exp_run_id);
    workload_request_.setWorkerInstanceIDs({graphml_id_});
    workload_request_.setWorkerInstancePaths({path_});
    workload_request_.setComponentInstanceIDS({comp_inst.getGraphmlID()});
    workload_request_.setComponentInstancePaths({comp_inst.getPath()});

    // Setup series
    workload_event_series_ = new WorkloadEventSeries(graphml_id_);
    workload_event_series_->setLabel(name_);

    connect(this, &WorkerInstanceData::requestData, ExperimentDataManager::manager(), &ExperimentDataManager::requestWorkerInstanceEvents);
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
 * @brief WorkerInstanceData::getWorkloadRequest
 * @return
 */
const WorkloadRequest& WorkerInstanceData::getWorkloadRequest() const
{
    return workload_request_;
}


/**
 * @brief WorkerInstanceData::addWorkloadEvents
 * @param events
 */
void WorkerInstanceData::addWorkloadEvents(const QVector<WorkloadEvent*>& events)
{
    qDebug() << "Received Workload Events#: " << events.size();
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


/**
 * @brief WorkerInstanceData::updateData
 * @param last_updated_time
 */
void WorkerInstanceData::updateData(qint64 last_updated_time)
{
    // NOTE: The requests need to be setup/updated before this signal is sent
    if (last_updated_time > last_updated_time_) {
        workload_request_.setTimeInterval({last_updated_time});
        last_updated_time_ = last_updated_time;
        emit requestData(*this);
    }
}
