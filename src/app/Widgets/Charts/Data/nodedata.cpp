#include "nodedata.h"
#include "../ExperimentDataManager/experimentdatamanager.h"

#include <QFuture>
#include <QFutureWatcher>

/**
 * @brief NodeData::NodeData
 * @param exp_run_id
 * @param node
 * @param parent
 */
NodeData::NodeData(quint32 exp_run_id, const AggServerResponse::Node& node, QObject *parent)
    : QObject(parent),
      experiment_run_id_(exp_run_id),
      last_updated_time_(0),
      hostname_(node.hostname),
      ip_(node.ip)
{
    // Construct Container data
    for (const auto& container : node.containers) {
        addContainerInstanceData(container);
    }

    // Setup the requests
    cpu_utilisation_request_.setExperimentRunID(exp_run_id);
    cpu_utilisation_request_.setNodeHostnames({hostname_});

    memory_utilisation_request_.setExperimentRunID(exp_run_id);
    memory_utilisation_request_.setNodeHostnames({hostname_});

    network_utilisation_request_.setExperimentRunID(exp_run_id);
    network_utilisation_request_.setNodeHostnames({hostname_});

    // Setup event series
    cpu_utilisation_series_ = new CPUUtilisationEventSeries(hostname_);
    cpu_utilisation_series_->setLabel(hostname_ + "_cpu");

    memory_utilisation_series_ = new MemoryUtilisationEventSeries(hostname_);
    memory_utilisation_series_->setLabel(hostname_ + "_mem");

    network_utilisation_series_ = new NetworkUtilisationEventSeries(hostname_);
    network_utilisation_series_->setLabel(hostname_ + "_net");

    connect(this, &NodeData::requestData, ExperimentDataManager::manager(), &ExperimentDataManager::requestNodeEvents);
    emit requestData(*this);
}


/**
 * @brief NodeData::getHostname
 * @return
 */
const QString& NodeData::getHostname() const
{
    return hostname_;
}


/**
 * @brief NodeData::getIP
 * @return
 */
const QString& NodeData::getIP() const
{
    return ip_;
}


/**
 * @brief NodeData::addContainerInstanceData
 * @param container
 */
void NodeData::addContainerInstanceData(const AggServerResponse::Container& container)
{
    auto container_data = container_inst_data_hash_.value(container.graphml_id, nullptr);
    if (container_data == nullptr) {
        container_data = new ContainerInstanceData(experiment_run_id_, container, this);
        container_inst_data_hash_.insert(container_data->getGraphmlID(), container_data);
    } else {
        container_data->updateData(container, last_updated_time_);
    }
}


/**
 * @brief NodeData::getContainerInstanceData
 * @return
 */
QList<ContainerInstanceData*> NodeData::getContainerInstanceData() const
{
    return container_inst_data_hash_.values();
}


/**
 * @brief NodeData::getCPUUtilisationRequest
 * @return
 */
const CPUUtilisationRequest& NodeData::getCPUUtilisationRequest() const
{
    return cpu_utilisation_request_;
}


/**
 * @brief NodeData::getMemoryUtilisationRequest
 * @return
 */
const MemoryUtilisationRequest& NodeData::getMemoryUtilisationRequest() const
{
    return memory_utilisation_request_;
}


/**
 * @brief NodeData::getNetworkUtilisationRequest
 * @return
 */
const NetworkUtilisationRequest& NodeData::getNetworkUtilisationRequest() const
{
    return network_utilisation_request_;
}


/**
 * @brief NodeData::addCPUUtilisationEvents
 * @param events
 */
void NodeData::addCPUUtilisationEvents(const QVector<CPUUtilisationEvent*>& events)
{
    cpu_utilisation_series_->addEvents(events);
}


/**
 * @brief NodeData::getCPUUtilisationSeries
 * @return
 */
CPUUtilisationEventSeries* NodeData::getCPUUtilisationSeries() const
{
    return cpu_utilisation_series_;
}


/**
 * @brief NodeData::addMemoryUtilisationEvents
 * @param events
 */
void NodeData::addMemoryUtilisationEvents(const QVector<MemoryUtilisationEvent*>& events)
{
    memory_utilisation_series_->addEvents(events);
}


/**
 * @brief NodeData::getMemoryUtilisationSeries
 * @return
 */
MemoryUtilisationEventSeries* NodeData::getMemoryUtilisationSeries() const
{
    return memory_utilisation_series_;
}


/**
 * @brief NodeData::addNetworkUtilisationEvents
 * @param events
 */
void NodeData::addNetworkUtilisationEvents(const QVector<NetworkUtilisationEvent*>& events)
{
    network_utilisation_series_->addEvents(events);
}


/**
 * @brief NodeData::getNetworkUtilisationSeries
 * @return
 */
NetworkUtilisationEventSeries* NodeData::getNetworkUtilisationSeries() const
{
    return network_utilisation_series_;
}


/**
 * @brief NodeData::updateData
 * This is called when the ExperimentRunData's last updated time has changed
 * It updates the container data and sets the new time interval for the particular event requests
 * that will be used by the ExperimentDataManager to update the corresponding event series
 * @param node
 * @param new_last_updated_time
 */
void NodeData::updateData(const AggServerResponse::Node& node, qint64 new_last_updated_time)
{
    // Setup/update the requests before sending the signal
    cpu_utilisation_request_.setTimeInterval({last_updated_time_, new_last_updated_time});
    memory_utilisation_request_.setTimeInterval({last_updated_time_, new_last_updated_time});
    network_utilisation_request_.setTimeInterval({last_updated_time_, new_last_updated_time});
    emit requestData(*this);

    // NOTE: Update last_updated_time_ before calling addContainerInstanceData
    last_updated_time_ = new_last_updated_time;
    for (const auto& container : node.containers) {
        addContainerInstanceData(container);
    }
}
