#include "nodedata.h"
#include "../ExperimentDataManager/experimentdatamanager.h"

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
    for (const auto& container : node.containers) {
        addContainerInstanceData(container);
    }

    setupRequests();
    setupSeries(node.interfaces);

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
 * @brief NodeData::getPortLifecycleSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getPortLifecycleSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getPortLifecycleSeries());
    }
    return series;
}

/**
 * @brief NodeData::getPortEventSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getPortEventSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getPortEventSeries());
    }
    return series;
}

/**
 * @brief NodeData::getWorkloadEventSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getWorkloadEventSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getWorkloadEventSeries());
    }
    return series;
}

/**
 * @brief NodeData::getCPUUtilisationRequest
 * @return
 */
const UtilisationRequest &NodeData::getCPUUtilisationRequest() const
{
    return cpu_utilisation_request_;
}

/**
 * @brief NodeData::getMemoryUtilisationRequest
 * @return
 */
const UtilisationRequest &NodeData::getMemoryUtilisationRequest() const
{
    return memory_utilisation_request_;
}

/**
 * @brief NodeData::getNetworkUtilisationRequest
 * @return
 */
const UtilisationRequest& NodeData::getNetworkUtilisationRequest() const
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
 * @throws std::runtime_error
 * @return
 */
QPointer<const MEDEA::EventSeries> NodeData::getCPUUtilisationSeries() const
{
	if (cpu_utilisation_series_ == nullptr) {
		throw std::runtime_error("NodeData::getCPUUtilisationSeries - CPU utilisation series is null");
	}
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
 * @throws std::runtime_error
 * @return
 */
QPointer<const MEDEA::EventSeries> NodeData::getMemoryUtilisationSeries() const
{
	if (memory_utilisation_series_ == nullptr) {
		throw std::runtime_error("NodeData::getMemoryUtilisationSeries - Memory utilisation series is null");
	}
	return memory_utilisation_series_;
}

/**
 * @brief NodeData::addNetworkUtilisationEvents
 * @param events
 * @throws std::runtime_error
 */
void NodeData::addNetworkUtilisationEvents(const QVector<NetworkUtilisationEvent*>& events)
{
    for (const auto& event : events) {
        const auto& mac_addr = event->getInterfaceMacAddress();
        auto series = network_utilisation_series_.value(mac_addr, nullptr);
        if (series == nullptr) {
            throw std::runtime_error("NodeData::addNetworkUtilisationEvents - There is no series for the provided interface mac address");
        }
        series->addEvent(event);
    }
}

/**
 * @brief NodeData::getNetworkUtilisationSeries
 * @throws std::runtime_error
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getNetworkUtilisationSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series_list;
    for (auto series : network_utilisation_series_.values()) {
        if (series == nullptr) {
            throw std::runtime_error("NodeData::getNetworkUtilisationSeries - Network utilisation series is null");
        }
        series_list.append(series);
    }
    return series_list;
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

/**
 * @brief NodeData::setupRequests
 */
void NodeData::setupRequests()
{
    cpu_utilisation_request_.setExperimentRunID(experiment_run_id_);
    cpu_utilisation_request_.setNodeHostnames({hostname_});

    memory_utilisation_request_.setExperimentRunID(experiment_run_id_);
    memory_utilisation_request_.setNodeHostnames({hostname_});

    network_utilisation_request_.setExperimentRunID(experiment_run_id_);
    network_utilisation_request_.setNodeHostnames({hostname_});
}

/**
 * @brief NodeData::setupSeries
 * @param interfaces
 */
void NodeData::setupSeries(const QVector<AggServerResponse::NetworkInterface>& interfaces)
{
    using namespace MEDEA;
    auto&& exp_run_id_str = QString::number(experiment_run_id_);
    auto&& node_id = hostname_ + exp_run_id_str;
    auto&& label = "[" + exp_run_id_str + "] " + hostname_;

    cpu_utilisation_series_ = new CPUUtilisationEventSeries(node_id + Event::GetChartDataKindString(ChartDataKind::CPU_UTILISATION));
    cpu_utilisation_series_->setLabel(label);
    cpu_utilisation_series_->setParent(this);

    memory_utilisation_series_ = new MemoryUtilisationEventSeries(node_id + Event::GetChartDataKindString(ChartDataKind::MEMORY_UTILISATION));
    memory_utilisation_series_->setLabel(label);
    memory_utilisation_series_->setParent(this);

    for (const auto& network_interface : interfaces) {
        const auto& mac_addr = network_interface.mac_address;
        auto&& series_id = hostname_ + mac_addr;
        auto series = new NetworkUtilisationEventSeries(series_id + exp_run_id_str);
        series->setLabel(label + "_" + network_interface.name);
        series->setParent(this);
        network_utilisation_series_.insert(mac_addr, series);
    }
}