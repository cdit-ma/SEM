#include "experimentdata.h"
#include <memory>

const int invalid_experiment_run_id = -1;

using namespace MEDEA;

/**
 * @brief ExperimentData::ExperimentData
 * @param experiment_name
 */
ExperimentData::ExperimentData(const QString& experiment_name)
    : experiment_name_(experiment_name) {}


/**
 * @brief ExperimentData::experiment_name
 * @return
 */
const QString &ExperimentData::experiment_name() const
{
    return experiment_name_;
}


/**
 * @brief ExperimentData::addExperimentRun
 * @param exp_run
 */
void ExperimentData::addExperimentRun(const AggServerResponse::ExperimentRun& exp_run)
{
    auto exp_run_id = exp_run.experiment_run_id;
    if (exp_run_id == invalid_experiment_run_id) {
        return;
    }

    ExperimentRunData&& exp_run_data = ExperimentRunData(ExperimentRunData::PassKey(),
                                                         static_cast<quint32>(exp_run_id),
                                                         exp_run.job_num,
                                                         exp_run.start_time,
                                                         exp_run.end_time,
                                                         exp_run.last_updated_time);

    experiment_runs_.push_back(exp_run_data);
}


/**
 * @brief ExperimentData::getExperimentRuns
 * @return
 */
const std::vector<ExperimentRunData>& ExperimentData::getExperimentRuns() const
{
    return experiment_runs_;
}
