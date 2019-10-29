#include "experimentrundata.h"

#include <QDateTime>
#include <QDebug>

/**
 * @brief MEDEA::ExperimentRunData::ExperimentRunData
 * @param experiment_name
 * @param experiemnt_run_id
 * @param job_num
 * @param start_time
 * @param end_time
 * @param last_updated_time
 */
MEDEA::ExperimentRunData::ExperimentRunData(PassKey, quint32 experiemnt_run_id, quint32 job_num, qint64 start_time, qint64 end_time, qint64 last_updated_time)
    : experiment_run_id_(experiemnt_run_id),
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
    // TODO - When the ChartManager is refactored, change ExperimentState to use quint32 and use a bool to check selected ExperimentRun instead
    auto exp_id = static_cast<qint32>(experiment_run_id_);

    if (exp_state.experiment_run_id == exp_id) {
        has_state_ = true;
        nodes_ = exp_state.nodes;
        components_ = exp_state.components;
        workers_ = exp_state.workers;
        port_connections_ = exp_state.port_connections;
    }
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
