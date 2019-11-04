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
 */
MEDEA::ExperimentRunData::ExperimentRunData(quint32 experiment_run_id, quint32 job_num, qint64 start_time, qint64 end_time, qint64 last_updated_time)
    : experiment_run_id_(experiment_run_id),
      job_num_(job_num),
      start_time_(start_time),
      end_time_(end_time),
      last_updated_time_(last_updated_time) {}


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
 * @brief MEDEA::ExperimentRunData::hasState
 * @return
 */
bool MEDEA::ExperimentRunData::hasState() const
{
    return has_state_;
}


/**
 * @brief MEDEA::ExperimentRunData::setExperimentState
 * @param exp_state
 */
void MEDEA::ExperimentRunData::setExperimentState(const AggServerResponse::ExperimentState& exp_state)
{
    // NOTE: Things that can change are the instances, port connections and markers; basically all the data except the nodes

    // If the state has already been set, clear the hashes before updating the state
    bool update = hasState();
    if (update) {
        clearState();
    }

    for (const auto& node : exp_state.nodes) {
        addNodeData(node);
    }
    for (const auto& p_c : exp_state.port_connections) {
        addPortConnection(p_c);
    }

    if (update) {
        emit dataChanged();
    }

    has_state_ = true;
}


/**
 * @brief MEDEA::ExperimentRunData::updateEndTime
 * @param time
 */
void MEDEA::ExperimentRunData::updateEndTime(qint64 time)
{
    end_time_ = time;
}


/**
 * @brief MEDEA::ExperimentRunData::updateLastUpdatedTime
 * @param time
 */
void MEDEA::ExperimentRunData::updateLastUpdatedTime(qint64 time)
{
    last_updated_time_ = time;
}


/**
 * @brief MEDEA::ExperimentRunData::getNodeData
 * @param hostname
 * @return
 */
QList<NodeData> MEDEA::ExperimentRunData::getNodeData(const QString& hostname) const
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
QList<PortConnectionData> MEDEA::ExperimentRunData::getPortConnectionData(int id) const
{
    if (id == -1) {
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
QList<MarkerSetData> MEDEA::ExperimentRunData::getMarkerSetData(int id) const
{
    if (id == -1) {
        return marker_set_hash_.values();
    } else {
        return marker_set_hash_.values(id);
    }
}


/**
 * @brief MEDEA::ExperimentRunData::addNodeData
 * @param node
 */
void MEDEA::ExperimentRunData::addNodeData(const AggServerResponse::Node& node)
{
    NodeData node_data(node);
    node_data_hash_.insert(node_data.getHostname(), node_data);
}


/**
 * @brief MEDEA::ExperimentRunData::addPortConnection
 * @param port_connection
 */
void MEDEA::ExperimentRunData::addPortConnection(const AggServerResponse::PortConnection& port_connection)
{
    PortConnectionData port_connection_data(port_connection);
    port_connection_hash_.insert(port_connection_data.getID(), port_connection_data);
}


/**
 * @brief MEDEA::ExperimentRunData::addMarkerSet
 * @param marker_name
 */
void MEDEA::ExperimentRunData::addMarkerSet(const QString& marker_name)
{
    MarkerSetData marker_set_data(marker_name);
    marker_set_hash_.insert(marker_set_data.getID(), marker_set_data);
}


/**
 * @brief MEDEA::ExperimentRunData::clearState
 */
void MEDEA::ExperimentRunData::clearState()
{
    node_data_hash_.clear();
    port_connection_hash_.clear();
    marker_set_hash_.clear();
    has_state_ = false;
}
