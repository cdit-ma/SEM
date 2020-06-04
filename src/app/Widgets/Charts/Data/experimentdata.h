#ifndef EXPERIMENTDATA_H
#define EXPERIMENTDATA_H

#include "experimentrundata.h"
#include "protomessagestructs.h"
#include <unordered_map>
#include <memory>

#include <QObject>

namespace MEDEA {

class ExperimentData : public QObject
{
    Q_OBJECT

public:
    ExperimentData(const QString& experiment_name, QObject* parent = nullptr);

    const QString& experiment_name() const;

    void addExperimentRun(const AggServerResponse::ExperimentRun& exp_run);
    ExperimentRunData& getExperimentRun(quint32 exp_run_id) const;

    void updateData(quint32 exp_run_id, const AggServerResponse::ExperimentState& exp_state);

signals:
    void requestData(quint32 exp_run_id);
    void experimentRunLastUpdateTimeChanged(quint32 exp_run_id, qint64 lat_update_time);

private slots:
    void experimentRunUpdated(qint64 last_updated_time);

private:
    QString experiment_name_;
    std::unordered_map<quint32, std::unique_ptr<ExperimentRunData>> experiment_run_map_;
};

}

#endif // EXPERIMENTDATA_H
