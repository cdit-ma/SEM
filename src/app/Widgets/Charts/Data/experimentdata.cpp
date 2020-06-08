#include "experimentdata.h"
#include <memory>

const int invalid_experiment_run_id = -1;

using namespace MEDEA;

/**
 * @brief ExperimentData::ExperimentData
 * @param experiment_name
 * @param parent
 */
ExperimentData::ExperimentData(QString experiment_name, QObject* parent)
    : QObject(parent),
      experiment_name_(std::move(experiment_name)) {}

/**
 * @brief ExperimentData::experiment_name
 * @return
 */
const QString& ExperimentData::experiment_name() const
{
    return experiment_name_;
}

/**
 * @brief ExperimentData::addExperimentRun
 * @param exp_run
 * @throws std::invalid_argument
 */
void ExperimentData::addExperimentRun(const AggServerResponse::ExperimentRun& exp_run)
{
    auto exp_run_id = exp_run.experiment_run_id;
    if (exp_run_id == invalid_experiment_run_id) {
        throw std::invalid_argument("ExperimentData::addExperimentRun - Invalid experiment run.");
    }

    auto exp_run_data = std::make_unique<ExperimentRunData>(experiment_name_,
                                                            exp_run_id,
                                                            exp_run.job_num,
                                                            exp_run.start_time,
                                                            exp_run.end_time,
                                                            exp_run.last_updated_time);

    connect(&(*exp_run_data), &ExperimentRunData::dataUpdated, this, &ExperimentData::experimentRunDataUpdated);

    // NOTE: emplace() - Inserts a new element into the container constructed in-place with the given args
    //  if there is no element with the key in the container
    experiment_run_map_.emplace(exp_run_id, std::move(exp_run_data));
}

/**
 * @brief ExperimentData::getExperimentRun
 * @param exp_run_id
 * @throws std::invalid_argument
 * @return
 */
MEDEA::ExperimentRunData& ExperimentData::getExperimentRun(quint32 exp_run_id) const
{
    auto&& exp_run = experiment_run_map_.at(exp_run_id);
    if (exp_run == nullptr) {
        throw std::invalid_argument("ExperimentData::getExperimentRun - No ExperimentRunData exists for experiment run id: " + std::to_string(exp_run_id));
    }
    return *exp_run;
}

/**
 * @brief ExperimentData::updateData
 * @param exp_run_id
 */
void ExperimentData::updateData(quint32 exp_run_id, const AggServerResponse::ExperimentState& exp_state)
{
    auto&& exp_run = experiment_run_map_.at(exp_run_id);
    if (exp_run != nullptr) {
        exp_run->updateData(exp_state);
    }
}

/**
 * @brief ExperimentData::experimentRunDataUpdated
 * This is called when the sender ExperimentRunData's last updated time has changed
 * @param last_updated_time - updated ExperimentRunData's last_updated_time
 */
void ExperimentData::experimentRunDataUpdated(qint64 last_updated_time)
{
    auto exp_run = qobject_cast<ExperimentRunData*>(sender());
    emit dataUpdated(exp_run->experiment_run_id(), last_updated_time);
}