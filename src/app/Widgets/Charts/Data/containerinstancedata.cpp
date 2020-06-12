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
    auto comp_inst_data = comp_inst_data_hash_.value(comp_inst.graphml_id, nullptr);
    if (comp_inst_data == nullptr) {
        comp_inst_data = new ComponentInstanceData(experiment_run_id_, comp_inst, this);
        comp_inst_data_hash_.insert(comp_inst_data->getGraphmlID(), comp_inst_data);
    } else {
        comp_inst_data->updateData(comp_inst, last_updated_time_);
    }
}

/**
 * @brief ContainerInstanceData::getComponentInstanceData
 * @return
 */
QList<ComponentInstanceData*> ContainerInstanceData::getComponentInstanceData() const
{
    return comp_inst_data_hash_.values();
}

/**
 * @brief ContainerInstanceData::getPortLifecycleSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getPortLifecycleSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData()) {
        series.append(comp_inst->getPortLifecycleSeries());
    }
    return series;
}

/**
 * @brief ContainerInstanceData::getPortEventSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getPortEventSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData()) {
        series.append(comp_inst->getPortEventSeries());
    }
    return series;
}

/**
 * @brief ContainerInstanceData::getWorkloadEventSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> ContainerInstanceData::getWorkloadEventSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& comp_inst : getComponentInstanceData()) {
        series.append(comp_inst->getWorkloadEventSeries());
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
