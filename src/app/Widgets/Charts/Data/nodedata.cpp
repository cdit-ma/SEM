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

    // Setup event series
    cpu_utilisation_series_ = new CPUUtilisationEventSeries(hostname_);
    cpu_utilisation_series_->setLabel(hostname_ + "_cpu");

    memory_utilisation_series_ = new MemoryUtilisationEventSeries(hostname_);
    memory_utilisation_series_->setLabel(hostname_ + "_mem");

    connect(this, &NodeData::requestData, ExperimentDataManager::manager(), &ExperimentDataManager::requestNodeEvents);
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
        qDebug() << "Created container data for: " << container.name;
        container_data = new ContainerInstanceData(experiment_run_id_, container, this);
        container_inst_data_hash_.insert(container_data->getGraphmlID(), container_data);
    }
    container_data->updateData(container, last_updated_time_);
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
const CPUUtilisationRequest &NodeData::getCPUUtilisationRequest() const
{
    return cpu_utilisation_request_;
}


/**
 * @brief NodeData::getMemoryUtilisationRequest
 * @return
 */
const MemoryUtilisationRequest &NodeData::getMemoryUtilisationRequest() const
{
    return memory_utilisation_request_;
}


/**
 * @brief NodeData::addCPUUtilisationEvents
 * @param events
 */
void NodeData::addCPUUtilisationEvents(const QVector<CPUUtilisationEvent*>& events)
{
    qDebug() << "Received CPU Events#: " << events.size();
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
    qDebug() << "Received Memory Events#: " << events.size();
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
 * @brief NodeData::updateData
 * @param node
 * @param last_updated_time
 */
void NodeData::updateData(const AggServerResponse::Node& node, qint64 last_updated_time)
{
    if (last_updated_time > last_updated_time_) {
        // Setup/update the requests before sending the signal
        cpu_utilisation_request_.setTimeInterval({last_updated_time});
        memory_utilisation_request_.setTimeInterval({last_updated_time});
        emit requestData(*this);

        last_updated_time_ = last_updated_time;
        for (const auto& container : node.containers) {
            addContainerInstanceData(container);
        }
    }
}
