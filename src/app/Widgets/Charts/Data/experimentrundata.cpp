#include "experimentrundata.h"

#include <QDateTime>
#include <QDebug>

/**
 * @brief MEDEA::ExperimentRunData::ExperimentRunData
 * @param experiment_run_id
 * @param job_num
 * @param start_time
 * @param end_time
 * @param last_updated_time
 * @param parent
 */
MEDEA::ExperimentRunData::ExperimentRunData(const QString& exp_name, quint32 experiment_run_id, quint32 job_num, qint64 start_time, qint64 end_time, qint64 last_updated_time, QObject *parent)
    : QObject(parent),
      experiment_name_(exp_name),
      experiment_run_id_(experiment_run_id),
      job_num_(job_num),
      start_time_(start_time),
      end_time_(end_time),
      last_updated_time_(last_updated_time) {}

/**
 * @brief MEDEA::ExperimentRunData::experiment_name
 * @return
 */
const QString& MEDEA::ExperimentRunData::experiment_name() const
{
    return experiment_name_;
}

/**
 * @brief MEDEA::ExperimentRunData::experiment_run_id
 * @return
 */
quint32 MEDEA::ExperimentRunData::experiment_run_id() const
{
    return experiment_run_id_;
}


/**
 * @brief MEDEA::ExperimentRunData::job_num
 * @return
 */
quint32 MEDEA::ExperimentRunData::job_num() const
{
    return job_num_;
}


/**
 * @brief MEDEA::ExperimentRunData::start_time
 * @return
 */
qint64 MEDEA::ExperimentRunData::start_time() const
{
    return start_time_;
}


/**
 * @brief MEDEA::ExperimentRunData::end_time
 * @return
 */
qint64 MEDEA::ExperimentRunData::end_time() const
{
    return end_time_;
}


/**
 * @brief MEDEA::ExperimentRunData::last_updated_time
 * @return
 */
qint64 MEDEA::ExperimentRunData::last_updated_time() const
{
    return last_updated_time_;
}


/**
 * @brief MEDEA::ExperimentRunData::getNodeData
 * @param hostname
 * @return
 */
QList<NodeData*> MEDEA::ExperimentRunData::getNodeData(const QString& hostname) const
{
    if (hostname.isEmpty()) {
        return node_data_hash_.values();
    } else {
        return node_data_hash_.values(hostname);
    }
}


/**
 * @brief MEDEA::ExperimentRunData::getPortConnectionData
 * @param id
 * @return
 */
QList<PortConnectionData*> MEDEA::ExperimentRunData::getPortConnectionData(const QString& id) const
{
    if (id.isEmpty()) {
        return port_connection_hash_.values();
    } else {
        return port_connection_hash_.values(id);
    }
}


/**
 * @brief MEDEA::ExperimentRunData::getMarkerSetData
 * @param id
 * @return
 */
QList<MarkerSetData*> MEDEA::ExperimentRunData::getMarkerSetData(int id) const
{
    if (id == -1) {
        return marker_set_hash_.values();
    } else {
        return marker_set_hash_.values(id);
    }
}


/**
 * @brief MEDEA::ExperimentRunData::updateData
 * @param exp_state
 */
void MEDEA::ExperimentRunData::updateData(const AggServerResponse::ExperimentState& exp_state)
{
    auto&& state_last_updated_time = exp_state.last_updated_time;
    if (state_last_updated_time > last_updated_time_) {
        // NOTE: last_updated_time_ is passed through to update the children data
        // Therefore, it needs to be updated before calling addNodeData and addPortConnection
        last_updated_time_ = state_last_updated_time;
        for (const auto& node : exp_state.nodes) {
            addNodeData(node);
        }
        for (const auto& p_c : exp_state.port_connections) {
            addPortConnection(p_c);
        }
        qDebug() << "exp run data updated";
        emit dataUpdated(last_updated_time_);

        // If the experiment run was live, send a signal to let listeners know if it has finished
        auto&& state_end_time = exp_state.end_time;
        if (state_end_time != end_time_) {
            end_time_ = state_end_time;
            emit experimentRunFinished(experiment_run_id_);
        }
    }
}


/**
 * @brief MEDEA::ExperimentRunData::addNodeData
 * @param node
 */
void MEDEA::ExperimentRunData::addNodeData(const AggServerResponse::Node& node)
{
    auto node_data = node_data_hash_.value(node.hostname, nullptr);
    if (node_data == nullptr) {
        node_data = new NodeData(experiment_run_id_, node, this);
        node_data_hash_.insert(node_data->getHostname(), node_data);
    } else {
        node_data->updateData(node, last_updated_time_);
    }
}


/**
 * @brief MEDEA::ExperimentRunData::addPortConnection
 * @param port_connection
 */
void MEDEA::ExperimentRunData::addPortConnection(const AggServerResponse::PortConnection& port_connection)
{
    auto&& connection_id = port_connection.from_port_graphml + port_connection.to_port_graphml;
    if (!port_connection_hash_.contains(connection_id)) {
        PortConnectionData* port_connection_data = new PortConnectionData(experiment_run_id_, port_connection, this);
        port_connection_hash_.insert(connection_id, port_connection_data);
    }
}


/**
 * @brief MEDEA::ExperimentRunData::addMarkerSet
 * @param marker_name
 */
void MEDEA::ExperimentRunData::addMarkerSet(const QString& marker_name)
{
    // TODO: Figure out where to construct this and what key to store it by
    MarkerSetData* marker_set_data = new MarkerSetData(experiment_run_id_, marker_name, this);
    marker_set_hash_.insert(marker_set_data->getID(), marker_set_data);
}
