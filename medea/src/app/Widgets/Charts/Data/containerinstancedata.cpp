#include "containerinstancedata.h"

/**
 * @brief ContainerInstanceData::ContainerInstanceData
 * @param exp_run_id
 * @param container
 * @param parent
 */
ContainerInstanceData::ContainerInstanceData(quint32 exp_run_id, const AggServerResponse::Container& container, QObject* parent)
    : QObject(parent),
      experiment_run_id_(exp_run_id),
      last_updated_time_(0),
      graphml_id_(container.graphml_id),
      name_(container.name),
      type_(container.type)
{
    for (const auto& comp_inst : container.component_instances) {
        addComponentInstanceData(comp_inst);
    }
}

/**
 * @brief ContainerInstanceData::getGraphmlID
 * @return
 */
const QString& ContainerInstanceData::getGraphmlID() const
{
    return graphml_id_;
}

/**
 * @brief ContainerInstanceData::getName
 * @return
 */
const QString& ContainerInstanceData::getName() const
{
    return name_;
}

/**
 * @brief ContainerInstanceData::getType
 * @return
 */
AggServerResponse::Container::ContainerType ContainerInstanceData::getType() const
{
    return type_;
}

/**
 * @brief ContainerInstanceData::addComponentInstanceData
 * @param comp_inst
 */
void ContainerInstanceData::addComponentInstanceData(const AggServerResponse::ComponentInstance& comp_inst)
{
    const auto& comp_inst_path = comp_inst.path;
    auto comp_inst_data = comp_inst_data_hash_.value(comp_inst_path, nullptr);
    if (comp_inst_data == nullptr) {
        comp_inst_data = new ComponentInstanceData(experiment_run_id_, comp_inst, this);
        comp_inst_data_hash_.insert(comp_inst_path, comp_inst_data);
        comp_name_hash_.insert(comp_inst.type, comp_inst_path);
    } else {
        comp_inst_data->updateData(comp_inst, last_updated_time_);
    }
}

/**
 * @brief ContainerInstanceData::getComponentInstanceData
 * @param path
 * @return
 */
QList<ComponentInstanceData*> ContainerInstanceData::getComponentInstanceData(const QString& path) const
{
    if (path.isEmpty()) {
        return comp_inst_data_hash_.values();
    } else {
        return comp_inst_data_hash_.values(path);
    }
}

/**
 * @brief ContainerInstanceData::getComponentInstanceData
 * @param comp_names
 * @param paths
 * @return
 */
QList<ComponentInstanceData*> ContainerInstanceData::getComponentInstanceData(const QStringList& comp_names, const QStringList& paths) const
{
    // No filters; return all ComponentInstanceData
    if (comp_names.isEmpty() && paths.isEmpty()) {
        return getComponentInstanceData();
    }

    auto filter_paths = paths;
    for (const auto& name : comp_names) {
        filter_paths.append(comp_name_hash_.values(name));
    }
    filter_paths.removeDuplicates();

    QList<ComponentInstanceData*> comp_inst_data_list;
    for (const auto& path : filter_paths) {
        comp_inst_data_list.append(getComponentInstanceData(path));
    }
    return comp_inst_data_list;
}

/**
 * @brief ContainerInstanceData::getPortLifecycleSeries
 * @param port_path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getPortLifecycleSeries(const QString& port_path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData()) {
        series.append(comp_inst->getPortLifecycleSeries(port_path));
    }
    return series;
}

/**
 * @brief ContainerInstanceData::getPortLifecycleSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param port_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getPortLifecycleSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData(comp_names, comp_inst_paths)) {
        series.append(comp_inst->getPortLifecycleSeries(port_paths));
    }
    return series;
}

/**
 * @brief ContainerInstanceData::getPortEventSeries
 * @param port_path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getPortEventSeries(const QString& port_path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData()) {
        series.append(comp_inst->getPortEventSeries(port_path));
    }
    return series;
}

/**
 * @brief ContainerInstanceData::getPortEventSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param port_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getPortEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData(comp_names, comp_inst_paths)) {
        series.append(comp_inst->getPortEventSeries(port_paths));
    }
    return series;
}

/**
 * @brief ContainerInstanceData::getWorkloadEventSeries
 * @param worker_inst_path
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getWorkloadEventSeries(const QString& worker_inst_path) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData()) {
        series.append(comp_inst->getWorkloadEventSeries(worker_inst_path));
    }
    return series;
}

/**
 * @brief ContainerInstanceData::getWorkloadEventSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param worker_inst_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getWorkloadEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& worker_inst_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData(comp_names, comp_inst_paths)) {
        series.append(comp_inst->getWorkloadEventSeries(worker_inst_paths));
    }
    return series;
}

/**
 * @brief ContainerInstanceData::updateData
 * This is called when the ExperimentRunData's last updated time has changed
 * It updates the children component instance data
 * @param container
 * @param new_last_updated_time
 */
void ContainerInstanceData::updateData(const AggServerResponse::Container& container, qint64 new_last_updated_time)
{
	// NOTE: Update last_updated_time_ before calling addComponentInstanceData
    last_updated_time_ = new_last_updated_time;
    for (const auto& comp_inst : container.component_instances) {
        addComponentInstanceData(comp_inst);
    }
}