#include "experimentdata.h"

const int invalid_experiment_run_id = -1;

using namespace MEDEA;

/**
 * @brief ExperimentData::ExperimentData
 * @param experiment_name
 * @param parent
 */
ExperimentData::ExperimentData(const QString& experiment_name, QObject* parent)
    : QObject(parent),
      experiment_name_(experiment_name) {}


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

    auto&& exp_run_data = std::make_unique<ExperimentRunData>(exp_run_id,
                                                              exp_run.job_num,
                                                              exp_run.start_time,
                                                              exp_run.end_time,
                                                              exp_run.last_updated_time);

    // TODO: We should figure out what the emplace/insert functions actually do
    experiment_run_map_.emplace(exp_run_id, std::move(exp_run_data));
}


/**
 * @brief ExperimentData::getExperimentRun
 * @param exp_run_id
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
    getExperimentRun(exp_run_id).updateData(exp_state);
}
