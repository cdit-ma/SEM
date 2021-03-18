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
    setupSeries(node.interfaces, node.gpus);

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
    const auto& container_id = container.graphml_id;
    auto container_data = container_inst_data_hash_.value(container_id, nullptr);
    if (container_data == nullptr) {
        container_data = new ContainerInstanceData(experiment_run_id_, container, this);
        container_inst_data_hash_.insert(container_id, container_data);
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
 * @param port_path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getPortLifecycleSeries(const QString& port_path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getPortLifecycleSeries(port_path));
    }
    return series;
}

/**
 * @brief NodeData::getPortLifecycleSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param port_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getPortLifecycleSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getPortLifecycleSeries(comp_names, comp_inst_paths, port_paths));
    }
    return series;
}

/**
 * @brief NodeData::getPortEventSeries
 * @param port_path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getPortEventSeries(const QString& port_path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getPortEventSeries(port_path));
    }
    return series;
}

/**
 * @brief NodeData::getPortEventSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param port_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getPortEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getPortEventSeries(comp_names, comp_inst_paths, port_paths));
    }
    return series;
}

/**
 * @brief NodeData::getWorkloadEventSeries
 * @param worker_inst_path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getWorkloadEventSeries(const QString& worker_inst_path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getWorkloadEventSeries(worker_inst_path));
    }
    return series;
}

/**
 * @brief NodeData::getWorkloadEventSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param worker_inst_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getWorkloadEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& worker_inst_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& container_inst : getContainerInstanceData()) {
        series.append(container_inst->getWorkloadEventSeries(comp_names, comp_inst_paths, worker_inst_paths));
    }
    return series;
}

/**
 * @brief NodeData::getCPUUtilisationRequest
 * @return
 */
const HardwareMetricRequest& NodeData::getCPUUtilisationRequest() const
{
    return cpu_utilisation_request_;
}

/**
 * @brief NodeData::getMemoryUtilisationRequest
 * @return
 */
const HardwareMetricRequest& NodeData::getMemoryUtilisationRequest() const
{
    return memory_utilisation_request_;
}

/**
 * @brief NodeData::getNetworkUtilisationRequest
 * @return
 */
const HardwareMetricRequest& NodeData::getNetworkUtilisationRequest() const
{
    return network_utilisation_request_;
}

/**
 * @brief NodeData::getGPUMetricsRequest
 * @return
 */
const HardwareMetricRequest& NodeData::getGPUMetricsRequest() const
{
    return gpu_metrics_request_;
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
 * @brief NodeData::addGPUComputeUtilisationEvents
 * @param events
 */
void NodeData::addGPUComputeUtilisationEvents(const QVector<GPUComputeUtilisationEvent*>& events)
{
    for (const auto& event : events) {
        const auto& device_index = event->getDeviceIndex();
        auto series = gpu_compute_utilisation_series_.value(device_index, nullptr);
        if (series == nullptr) {
            throw std::runtime_error("NodeData::addGPUComputeUtilisationEvents - There is no series for the gpu device");
        }
        series->addEvent(event);
    }
}

/**
 * @brief NodeData::getGPUComputeUtilisationSeries
 * @throws std::runtime_error
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getGPUComputeUtilisationSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series_list;
    for (auto series : gpu_compute_utilisation_series_.values()) {
        if (series == nullptr) {
            throw std::runtime_error("NodeData::getGPUComputeUtilisationSeries - GPU compute utilisation series is null");
        }
        series_list.append(series);
    }
    return series_list;
}

/**
 * @brief NodeData::addGPUMemoryUtilisationEvents
 * @param events
 */
void NodeData::addGPUMemoryUtilisationEvents(const QVector<GPUMemoryUtilisationEvent*>& events)
{
    for (const auto& event : events) {
        const auto& device_index = event->getDeviceIndex();
        auto series = gpu_memory_utilisation_series_.value(device_index, nullptr);
        if (series == nullptr) {
            throw std::runtime_error("NodeData::addGPUMemoryUtilisationEvents - There is no series for the gpu device");
        }
        series->addEvent(event);
    }
}

/**
 * @brief NodeData::getGPUMemoryUtilisationSeries
 * @throws std::runtime_error
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getGPUMemoryUtilisationSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series_list;
    for (auto series : gpu_memory_utilisation_series_.values()) {
        if (series == nullptr) {
            throw std::runtime_error("NodeData::getGPUMemoryUtilisationSeries - GPU memory utilisation series is null");
        }
        series_list.append(series);
    }
    return series_list;
}

/**
 * @brief NodeData::addGPUTemperatureEvents
 * @param events
 */
void NodeData::addGPUTemperatureEvents(const QVector<GPUTemperatureEvent*>& events)
{
    for (const auto& event : events) {
        const auto& device_index = event->getDeviceIndex();
        auto series = gpu_temperature_series_.value(device_index, nullptr);
        if (series == nullptr) {
            throw std::runtime_error("NodeData::addGPUTemperatureEvents - There is no series for the gpu device");
        }
        series->addEvent(event);
    }
}

/**
 * @brief NodeData::getGPUTemperatureSeries
 * @throws std::runtime_error
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> NodeData::getGPUTemperatureSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series_list;
    for (auto series : gpu_temperature_series_.values()) {
        if (series == nullptr) {
            throw std::runtime_error("NodeData::getGPUTemperatureSeries - GPU temperature series is null");
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

    gpu_metrics_request_.setExperimentRunID(experiment_run_id_);
    gpu_metrics_request_.setNodeHostnames({hostname_});
}

/**
 * @brief NodeData::setupSeries
 * @param interfaces
 * @param gpu_devices
 */
void NodeData::setupSeries(const QVector<AggServerResponse::NetworkInterface>& interfaces,
                           const QVector<AggServerResponse::GPUDevice>& gpu_devices)
{
    using namespace MEDEA;
    const auto exp_run_id_str = QString::number(experiment_run_id_);
    const auto node_id = hostname_ + exp_run_id_str;
    const auto default_label = "[" + exp_run_id_str + "] " + hostname_;

    cpu_utilisation_series_ = new CPUUtilisationEventSeries(node_id + Event::GetChartDataKindString(ChartDataKind::CPU_UTILISATION));
    cpu_utilisation_series_->setLabel(default_label);
    cpu_utilisation_series_->setParent(this);

    memory_utilisation_series_ = new MemoryUtilisationEventSeries(node_id + Event::GetChartDataKindString(ChartDataKind::MEMORY_UTILISATION));
    memory_utilisation_series_->setLabel(default_label);
    memory_utilisation_series_->setParent(this);

    for (const auto& network_interface : interfaces) {
        const auto& mac_addr = network_interface.mac_address;
        //auto&& series_id = hostname_ + mac_addr;
        //auto series = new NetworkUtilisationEventSeries(series_id + exp_run_id_str);
        auto series = new NetworkUtilisationEventSeries(node_id + mac_addr);
        series->setLabel(default_label + "_" + network_interface.name);
        series->setParent(this);
        network_utilisation_series_.insert(mac_addr, series);
    }

    for (const auto& gpu : gpu_devices) {
        const auto& device_index = gpu.gpu_device_index;
        auto device_index_str = QString::number(device_index);
        auto&& series_id = node_id + device_index_str;

        auto comp_util_series = new GPUComputeUtilisationSeries(series_id + Event::GetChartDataKindString(ChartDataKind::GPU_COMPUTE_UTILISATION));
        comp_util_series->setLabel(default_label + "_" + device_index_str);
        comp_util_series->setParent(this);

        auto mem_util_series = new GPUMemoryUtilisationSeries(series_id + Event::GetChartDataKindString(ChartDataKind::GPU_MEMORY_UTILISATION));
        mem_util_series->setLabel(default_label + "_" + device_index_str);
        mem_util_series->setParent(this);

        auto temp_series = new GPUTemperatureSeries(series_id + Event::GetChartDataKindString(ChartDataKind::GPU_TEMPERATURE));
        temp_series->setLabel(default_label + "_" + device_index_str);
        temp_series->setParent(this);

        gpu_compute_utilisation_series_.insert(device_index, comp_util_series);
        gpu_memory_utilisation_series_.insert(device_index, mem_util_series);
        gpu_temperature_series_.insert(device_index, temp_series);
    }
}