#include "experimentrundata.h"

/**
 * @brief MEDEA::ExperimentRunData::ExperimentRunData
 * @param experiment_run_id
 * @param job_num
 * @param start_time
 * @param end_time
 * @param last_updated_time
 * @param parent
 */
MEDEA::ExperimentRunData::ExperimentRunData(QString exp_name, quint32 experiment_run_id, quint32 job_num, qint64 start_time, qint64 end_time, qint64 last_updated_time, QObject *parent)
    : QObject(parent),
      experiment_name_(std::move(exp_name)),
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
        // NOTE: QHash.values(const Key &key) overload will be removed from Qt 5.15 onwards
        // TODO: Fix when fixing other deprecations
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
 * @return
 */
QList<MarkerSetData*> MEDEA::ExperimentRunData::getMarkerSetData() const
{
    return marker_set_hash_.values();
}

/**
 * @brief MEDEA::ExperimentRunData::getMarkerSetData
 * @param marker_name
 * @return
 */
MarkerSetData* MEDEA::ExperimentRunData::getMarkerSetData(const QString& marker_name) const
{
    return marker_set_hash_.value(marker_name, nullptr);
}

/**
 * @brief MEDEA::ExperimentRunData::getPortLifecycleSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getPortLifecycleSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData()) {
        series.append(node_data->getPortLifecycleSeries());
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getPortLifecycleSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param port_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getPortLifecycleSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData()) {
        series.append(node_data->getPortLifecycleSeries(comp_names, comp_inst_paths, port_paths));
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getPortEventSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getPortEventSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData()) {
        series.append(node_data->getPortEventSeries());
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getPortEventSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param port_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getPortEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData()) {
        series.append(node_data->getPortEventSeries(comp_names, comp_inst_paths, port_paths));
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getWorkloadEventSeries
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getWorkloadEventSeries() const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData()) {
        series.append(node_data->getWorkloadEventSeries());
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getWorkloadEventSeries
 * @param comp_names
 * @param comp_inst_paths
 * @param worker_inst_paths
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getWorkloadEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& worker_inst_paths) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData()) {
        series.append(node_data->getWorkloadEventSeries(comp_names, comp_inst_paths, worker_inst_paths));
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getCPUUtilisationSeries
 * @param hostname
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getCPUUtilisationSeries(const QString& hostname) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData(hostname)) {
        series.append(node_data->getCPUUtilisationSeries());
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getCPUUtilisationSeries
 * @param hostname
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getCPUUtilisationSeries(const QStringList& hostnames) const
{
    if (hostnames.isEmpty()) {
        return getCPUUtilisationSeries();
    }
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& hostname : hostnames) {
        series.append(getCPUUtilisationSeries(hostname));
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getMemoryUtilisationSeries
 * @param hostname
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getMemoryUtilisationSeries(const QString& hostname) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData(hostname)) {
        series.append(node_data->getMemoryUtilisationSeries());
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getMemoryUtilisationSeries
 * @param hostnames
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getMemoryUtilisationSeries(const QStringList& hostnames) const
{
    if (hostnames.isEmpty()) {
        return getMemoryUtilisationSeries();
    }
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& hostname : hostnames) {
        series.append(getMemoryUtilisationSeries(hostname));
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getNetworkUtilisationSeries
 * @param hostname
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getNetworkUtilisationSeries(const QString& hostname) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& node_data : getNodeData(hostname)) {
        series.append(node_data->getNetworkUtilisationSeries());
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getNetworkUtilisationSeries
 * @param hostnames
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getNetworkUtilisationSeries(const QStringList& hostnames) const
{
    if (hostnames.isEmpty()) {
        return getNetworkUtilisationSeries();
    }
    QList<QPointer<const MEDEA::EventSeries>> series;
    for (const auto& hostname : hostnames) {
        series.append(getNetworkUtilisationSeries(hostname));
    }
    return series;
}

/**
 * @brief MEDEA::ExperimentRunData::getMarkerEventSeries
 * @param marker_name
 * @return
 */
QList<QPointer<const MEDEA::EventSeries>> MEDEA::ExperimentRunData::getMarkerEventSeries(const QString& marker_name) const
{
    QList<QPointer<const MEDEA::EventSeries>> series;
    if (marker_name.isEmpty()) {
        for (const auto& marker_set_data : getMarkerSetData()) {
            series.append(marker_set_data->getMarkerEventSeries());
        }
    } else {
        auto marker_data = getMarkerSetData(marker_name);
        if (marker_data != nullptr) {
            series.append(marker_data->getMarkerEventSeries());
        }
    }
    return series;
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
        //  Therefore, it needs to be updated before calling addNodeData and addPortConnectionData
        last_updated_time_ = state_last_updated_time;
        for (const auto& node : exp_state.nodes) {
            addNodeData(node);
        }
        for (const auto& p_c : exp_state.port_connections) {
            addPortConnectionData(p_c);
        }
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
 * MEDEA::ExperimentRunData::addMarkerSetData
 * @param marker_name
 * @throws std::invalid_argument
 * @return
 */
MarkerSetData& MEDEA::ExperimentRunData::addMarkerSetData(const QString& marker_name)
{
    if (marker_set_hash_.contains(marker_name)) {
        throw std::logic_error("ExperimentRunData::addMarkerSetData - A MarkerSetData with name "
                               + marker_name.toStdString() + " already exists.");
    }
    auto marker_set_data = new MarkerSetData(experiment_run_id_, marker_name, this);
    marker_set_hash_.insert(marker_name, marker_set_data);
    return *marker_set_data;
}

/**
 * @brief MEDEA::ExperimentRunData::addNodeData
 * @param node
 */
void MEDEA::ExperimentRunData::addNodeData(const AggServerResponse::Node& node)
{
    const auto& hostname = node.hostname;
    auto node_data = node_data_hash_.value(hostname, nullptr);
    if (node_data == nullptr) {
        node_data = new NodeData(experiment_run_id_, node, this);
        node_data_hash_.insert(hostname, node_data);
    } else {
        node_data->updateData(node, last_updated_time());
    }
}

/**
 * @brief MEDEA::ExperimentRunData::addPortConnectionData
 * @param port_connection
 */
void MEDEA::ExperimentRunData::addPortConnectionData(const AggServerResponse::PortConnection& port_connection)
{
    auto&& connection_id = port_connection.from_port_graphml + port_connection.to_port_graphml;
    if (!port_connection_hash_.contains(connection_id)) {
        auto port_connection_data = new PortConnectionData(experiment_run_id_, port_connection, this);
        port_connection_hash_.insert(connection_id, port_connection_data);
    }
}
