#ifndef EXPERIMENTDATA_H
#define EXPERIMENTDATA_H

#include "experimentrundata.h"
#include "Events/protomessagestructs.h"
#include <vector>

namespace MEDEA {

class ExperimentData {

public:
    ExperimentData(const QString& experiment_name);

    const QString& experiment_name() const;

    void addExperimentRun(const AggServerResponse::ExperimentRun& exp_run);
    const std::vector<ExperimentRunData> &getExperimentRuns() const;

private:
    const QString experiment_name_;
    std::vector<ExperimentRunData> experiment_runs_;

};

}

#endif // EXPERIMENTDATA_H
