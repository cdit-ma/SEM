#ifndef EXPERIMENTDATA_H
#define EXPERIMENTDATA_H

#include "experimentrundata.h"
#include "protomessagestructs.h"
#include <unordered_map>

namespace MEDEA {

class ExperimentData {

public:
    ExperimentData(const QString& experiment_name);

    const QString& experiment_name() const;

    void addExperimentRun(const AggServerResponse::ExperimentRun& exp_run);
    ExperimentRunData& getExperimentRun(quint32 exp_run_id) const;
    //const std::vector<ExperimentRunData> &getExperimentRuns() const;

private:
    QString experiment_name_;
    std::unordered_map<quint32, std::unique_ptr<ExperimentRunData>> experiment_run_map_;
    //std::vector<ExperimentRunData> experiment_runs_;
};

}

#endif // EXPERIMENTDATA_H
