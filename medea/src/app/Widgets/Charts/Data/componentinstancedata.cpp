#include "componentinstancedata.h"

/**
 * @brief ComponentInstanceData::ComponentInstanceData
 * @param exp_run_id
 * @param component_instance
 * @param parent
 */
ComponentInstanceData::ComponentInstanceData(quint32 exp_run_id, const AggServerResponse::ComponentInstance& component_instance, QObject *parent)
	: QObject(parent),
	  experiment_run_id_(exp_run_id),
	  last_updated_time_(0),
	  graphml_id_(component_instance.graphml_id),
	  name_(component_instance.name),
	  path_(component_instance.path),
	  type_(component_instance.type)
{
    for (const auto& port : component_instance.ports) {
        addPortInstanceData(port);
    }
    for (const auto& worker_inst : component_instance.worker_instances) {
        addWorkerInstanceData(worker_inst);
    }
}

/**
 * @brief ComponentInstanceData::getGraphmlID
 * @return
 */
const QString& ComponentInstanceData::getGraphmlID() const
{
    return graphml_id_;
}

/**
 * @brief ComponentInstanceData::getName
 * @return
 */
const QString& ComponentInstanceData::getName() const
{
    return name_;
}

/**
 * @brief ComponentInstanceData::getPath
 * @return
 */
const QString& ComponentInstanceData::getPath() const
{
    return path_;
}

/**
 * @brief ComponentInstanceData::getType
 * @return
 */
const QString& ComponentInstanceData::getType() const
{
    return type_;
}

/**
 * @brief ComponentInstanceData::addPortInstanceData
 * @param port
 */
void ComponentInstanceData::addPortInstanceData(const AggServerResponse::Port& port)
{
    const auto& port_inst_path = port.path;
    auto port_data = port_inst_data_hash_.value(port_inst_path, nullptr);
    if (port_data == nullptr) {
        port_data = new PortInstanceData(experiment_run_id_, *this, port, this);
        port_inst_data_hash_.insert(port_inst_path, port_data);
    } else {
        port_data->updateData(last_updated_time_);
    }
}

/**
 * @brief ComponentInstanceData::getPortInstanceData
 * @param path
 * @return
 */
QList<PortInstanceData*> ComponentInstanceData::getPortInstanceData(const QString& path) const
{
    if (path.isEmpty()) {
        return port_inst_data_hash_.values();
    } else {
        return port_inst_data_hash_.values(path);
    }
}

/**
 * @brief ComponentInstanceData::getPortInstanceData
 * @param paths
 * @return
 */
QList<PortInstanceData*> ComponentInstanceData::getPortInstanceData(const QStringList& paths) const
{
    if (paths.isEmpty()) {
        return getPortInstanceData();
    }
    QList<PortInstanceData*> port_inst_data_list;
    for (const auto& path : paths) {
        port_inst_data_list.append(getPortInstanceData(path));
    }
    return port_inst_data_list;
}

/**
 * @brief ComponentInstanceData::addWorkerInstanceData
 * @param worker_instance
 */
void ComponentInstanceData::addWorkerInstanceData(const AggServerResponse::WorkerInstance& worker_instance)
{
    const auto& worker_inst_path = worker_instance.path;
    auto worker_inst_data = worker_inst_data_hash_.value(worker_inst_path, nullptr);
    if (worker_inst_data == nullptr) {
        worker_inst_data = new WorkerInstanceData(experiment_run_id_, *this, worker_instance, this);
        worker_inst_data_hash_.insert(worker_inst_path, worker_inst_data);
    } else {
        worker_inst_data->updateData(last_updated_time_);
    }
}

/**
 * @brief ComponentInstanceData::getWorkerInstanceData
 * @param path
 * @return
 */
QList<WorkerInstanceData*> ComponentInstanceData::getWorkerInstanceData(const QString& path) const
{
    if (path.isEmpty()) {
        return worker_inst_data_hash_.values();
    } else {
        return worker_inst_data_hash_.values(path);
    }
}

/**
 * @brief ComponentInstanceData::getWorkerInstanceData
 * @param paths
 * @return
 */
QList<WorkerInstanceData*> ComponentInstanceData::getWorkerInstanceData(const QStringList& paths) const
{
    if (paths.isEmpty()) {
        return getWorkerInstanceData();
    }
    QList<WorkerInstanceData*> worker_inst_data_list;
    for (const auto& path : paths) {
        worker_inst_data_list.append(getWorkerInstanceData(path));
    }
    return worker_inst_data_list;
}

/**
 * @brief ComponentInstanceData::getPortLifecycleSeries
 * @param path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ComponentInstanceData::getPortLifecycleSeries(const QString& path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& port_inst : getPortInstanceData(path)) {
        series.append(port_inst->getPortLifecycleSeries());
    }
    return series;
}

/**
 * @brief ComponentInstanceData::getPortLifecycleSeries
 * @param paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ComponentInstanceData::getPortLifecycleSeries(const QStringList& paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& port_inst : getPortInstanceData(paths)) {
        series.append(port_inst->getPortLifecycleSeries());
    }
    return series;
}

/**
 * @brief ComponentInstanceData::getPortEventSeries
 * @param path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ComponentInstanceData::getPortEventSeries(const QString& path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& port_inst : getPortInstanceData(path)) {
        series.append(port_inst->getPortEventSeries());
    }
    return series;
}

/**
 * @brief ComponentInstanceData::getPortEventSeries
 * @param paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ComponentInstanceData::getPortEventSeries(const QStringList& paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& port_inst : getPortInstanceData(paths)) {
        series.append(port_inst->getPortEventSeries());
    }
    return series;
}

/**
 * @brief ComponentInstanceData::getWorkloadEventSeries
 * @param path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ComponentInstanceData::getWorkloadEventSeries(const QString& path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& worker_inst : getWorkerInstanceData(path)) {
        series.append(worker_inst->getWorkloadEventSeries());
    }
    return series;
}

/**
 * @brief ComponentInstanceData::getWorkloadEventSeries
 * @param paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ComponentInstanceData::getWorkloadEventSeries(const QStringList& paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& worker_inst : getWorkerInstanceData(paths)) {
        series.append(worker_inst->getWorkloadEventSeries());
    }
    return series;
}

/**
 * @brief ComponentInstanceData::updateData
 * This is called when the ExperimentRunData's last updated time has changed
 * It updates the children port data and worker instance data
 * @param component_instance
 * @param new_last_updated_time
 */
void ComponentInstanceData::updateData(const AggServerResponse::ComponentInstance& component_instance, qint64 new_last_updated_time)
{
    // NOTE: Update last_updated_time_ before calling addPortInstanceData and addWorkerInstanceData
    last_updated_time_ = new_last_updated_time;
    for (const auto& port : component_instance.ports) {
        addPortInstanceData(port);
    }
    for (const auto& worker_inst : component_instance.worker_instances) {
        addWorkerInstanceData(worker_inst);
    }
}