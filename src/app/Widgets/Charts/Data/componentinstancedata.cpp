#include "componentinstancedata.h"

/**
 * @brief ComponentInstanceData::ComponentInstanceData
 * @param component_instance
 */
ComponentInstanceData::ComponentInstanceData(const AggServerResponse::ComponentInstance& component_instance)
    : graphml_id_(component_instance.graphml_id),
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
 * @throws std::invalid_argument
 */
void ComponentInstanceData::addPortInstanceData(const AggServerResponse::Port& port)
{
    if (port_inst_data_hash_.contains(port.graphml_id)) {
        throw std::invalid_argument("ComponentInstanceData::addPortInstanceData - Attempting to add a port that already exists.");
    }
    PortInstanceData port_data(port);
    port_inst_data_hash_.insert(port_data.getGraphmlID(), port_data);
}


/**
 * @brief ComponentInstanceData::getPortInstanceData
 * @return
 */
QList<PortInstanceData> ComponentInstanceData::getPortInstanceData() const
{
    return port_inst_data_hash_.values();
}


/**
 * @brief ComponentInstanceData::addWorkerInstanceData
 * @param worker_instance
 * @throws std::invalid_argument
 */
void ComponentInstanceData::addWorkerInstanceData(const AggServerResponse::WorkerInstance& worker_instance)
{
    if (worker_inst_data_hash_.contains(worker_instance.graphml_id)) {
        throw std::invalid_argument("ComponentInstanceData::addWorkerInstanceData - Attempting to add a worker instance that already exists.");
    }
    WorkerInstanceData worker_inst_data(worker_instance);
    worker_inst_data_hash_.insert(worker_inst_data.getGraphmlID(), worker_inst_data);
}


/**
 * @brief ComponentInstanceData::getWorkerInstanceData
 * @return
 */
QList<WorkerInstanceData> ComponentInstanceData::getWorkerInstanceData() const
{
    return worker_inst_data_hash_.values();
}
