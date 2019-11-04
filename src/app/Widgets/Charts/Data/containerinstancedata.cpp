#include "containerinstancedata.h"

/**
 * @brief ContainerInstanceData::ContainerInstanceData
 * @param container
 */
ContainerInstanceData::ContainerInstanceData(const AggServerResponse::Container& container)
    : graphml_id_(container.graphml_id),
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
const QString &ContainerInstanceData::getName() const
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
 * @throws std::invalid_argument
 */
void ContainerInstanceData::addComponentInstanceData(const AggServerResponse::ComponentInstance& comp_inst)
{
    if (comp_inst_data_hash_.contains(comp_inst.graphml_id)) {
        throw std::invalid_argument("ContainerInstanceData::addComponentInstanceData - Attempting to add a component instance that already exists.");
    }
    ComponentInstanceData comp_inst_data(comp_inst);
    comp_inst_data_hash_.insert(comp_inst_data.getGraphmlID(), comp_inst_data);
}


/**
 * @brief ContainerInstanceData::getComponentInstanceData
 * @return
 */
QList<ComponentInstanceData> ContainerInstanceData::getComponentInstanceData() const
{
    return comp_inst_data_hash_.values();
}
