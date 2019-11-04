#include "nodedata.h"

/**
 * @brief NodeData::NodeData
 * @param node
 */
NodeData::NodeData(const AggServerResponse::Node& node)
    : hostname_(node.hostname),
      ip_(node.ip)
{
    for (const auto& container : node.containers) {
        addContainerInstanceData(container);
    }
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
 * @throws std::invalid_argument
 */
void NodeData::addContainerInstanceData(const AggServerResponse::Container& container)
{
    if (container_inst_data_hash_.contains(container.graphml_id)) {
        throw std::invalid_argument("NodeData::addContainerInstanceData - Attempting to add a container that already exists.");
    }
    ContainerInstanceData container_data(container);
    container_inst_data_hash_.insert(container_data.getGraphmlID(), container_data);
}


/**
 * @brief NodeData::getContainerInstanceData
 * @return
 */
QList<ContainerInstanceData> NodeData::getContainerInstanceData() const
{
    return container_inst_data_hash_.values();
}


/**
 * @brief NodeData::addCPUUtilisationEvents
 * @param events
 */
void NodeData::addCPUUtilisationEvents(const QVector<CPUUtilisationEvent*>& events)
{
    if (events.isEmpty()) {
        return;
    }

    if (cpu_utilisation_series_ == nullptr) {
        cpu_utilisation_series_ = new CPUUtilisationEventSeries(getHostname());
        cpu_utilisation_series_->setProperty("label", getHostname() + "_cpu");
    }

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
    if (events.isEmpty()) {
        return;
    }

    if (memory_utilisation_series_ == nullptr) {
        memory_utilisation_series_ = new MemoryUtilisationEventSeries(getHostname());
        memory_utilisation_series_->setProperty("label", getHostname() + "_mem");
    }

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
