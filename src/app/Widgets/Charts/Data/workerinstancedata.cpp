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
    setupRequests();

    connect(this, &WorkerInstanceData::requestData, ExperimentDataManager::manager(), &ExperimentDataManager::requestWorkerInstanceEvents);
    emit requestData(*this);
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
    for (const auto& event : events) {
        // TODO: Once the request filtering is fixed, this check can be removed
        if (event->getWorkerInstPath() != path_) {
            continue;
        }
        auto&& series_id = graphml_id_ + event->getFunctionName();
        auto series = workload_event_series_.value(series_id, nullptr);
        if (series == nullptr) {
            auto&& series_name = name_ + "_" + graphml_id_ + "_" + event->getFunctionName();
            series = &setupSeries(series_id, series_name);
        }
        series->addEvent(event);
    }
}

/**
 * @brief WorkerInstanceData::getWorkloadEventSeries
 * @throws std::runtime_error
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> WorkerInstanceData::getWorkloadEventSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series_list;
    for (auto series : workload_event_series_.values()) {
        if (series == nullptr) {
            throw std::runtime_error("WorkerInstanceData::getWorkloadEventSeries - Workload event series is null");
        }
        series_list.append(series);
    }
    return series_list;
}

/**
 * @brief WorkerInstanceData::updateData
 * This is called when the ExperimentRunData's last updated time has changed
 * It sets the new time interval for the particular event requests that will
 * be used by the ExperimentDataManager to update the corresponding event series
 * @param new_last_updated_time
 */
void WorkerInstanceData::updateData(qint64 new_last_updated_time)
{
    // Setup/update the requests before sending the signal
    workload_request_.setTimeInterval({last_updated_time_, new_last_updated_time});
    last_updated_time_ = new_last_updated_time;
    emit requestData(*this);
}

/**
 * @brief WorkerInstanceData::setupRequests
 */
void WorkerInstanceData::setupRequests()
{
    // TODO: There is currently a bug with the filtering
    //  We get events with different worker instance paths
    workload_request_.setExperimentRunID(experiment_run_id_);
    workload_request_.setWorkerInstanceIDs({graphml_id_});
    workload_request_.setWorkerInstancePaths({path_});

    // INSPECT: These extra request fields are returning unexpected events
    //  When they are added, they also return workload events from other children ports of the parenting ComponentInstance
    // TODO: Find out what is causing this
    // Update: Filtering doesn’t work in the aggregation broker for certain combinations of filter strings on certain types of events, particularly workload events
    //workload_request_.setComponentInstanceIDS({comp_inst.getGraphmlID()});
    //workload_request_.setComponentInstancePaths({comp_inst.getPath()});
}

/**
 * @brief WorkerInstanceData::setupSeries
 * @param series_id
 * @param series_name
 * @return
 */
WorkloadEventSeries& WorkerInstanceData::setupSeries(const QString& series_id, const QString& series_name)
{
    auto&& exp_run_id_str = QString::number(experiment_run_id_);

    auto series = new WorkloadEventSeries(series_id + exp_run_id_str);
    series->setLabel("[" + exp_run_id_str + "] " + series_name);
    series->setParent(this);

    workload_event_series_.insert(series_id, series);
    return *series;
}
