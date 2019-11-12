#include "componentinstancedata.h"

#include <QDebug>

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
    auto port_data = port_inst_data_hash_.value(port.graphml_id, nullptr);
    if (port_data == nullptr) {
        qDebug() << "\nCreated data for: " << port.name;
        port_data = new PortInstanceData(experiment_run_id_, *this, port, this);
        port_inst_data_hash_.insert(port_data->getGraphmlID(), port_data);
    }
    port_data->updateData(last_updated_time_);
}


/**
 * @brief ComponentInstanceData::getPortInstanceData
 * @return
 */
QList<PortInstanceData*> ComponentInstanceData::getPortInstanceData() const
{
    return port_inst_data_hash_.values();
}


/**
 * @brief ComponentInstanceData::addWorkerInstanceData
 * @param worker_instance
 */
void ComponentInstanceData::addWorkerInstanceData(const AggServerResponse::WorkerInstance& worker_instance)
{
    auto worker_inst_data = worker_inst_data_hash_.value(worker_instance.graphml_id, nullptr);
    if (worker_inst_data == nullptr) {
        qDebug() << "\nCreated data for: " << worker_instance.name;
        worker_inst_data = new WorkerInstanceData(experiment_run_id_, *this, worker_instance, this);
        worker_inst_data_hash_.insert(worker_inst_data->getGraphmlID(), worker_inst_data);
    }
    worker_inst_data->updateData(last_updated_time_);
}


/**
 * @brief ComponentInstanceData::getWorkerInstanceData
 * @return
 */
QList<WorkerInstanceData*> ComponentInstanceData::getWorkerInstanceData() const
{
    return worker_inst_data_hash_.values();
}


/**
 * @brief ComponentInstanceData::updateData
 * @param component_instance
 * @param last_updated_time
 */
void ComponentInstanceData::updateData(const AggServerResponse::ComponentInstance& component_instance, qint64 last_updated_time)
{
    if (last_updated_time > last_updated_time_) {
        last_updated_time_ = last_updated_time;
        for (const auto& port : component_instance.ports) {
            addPortInstanceData(port);
        }
        for (const auto& worker_inst : component_instance.worker_instances) {
            addWorkerInstanceData(worker_inst);
        }
    }
}
