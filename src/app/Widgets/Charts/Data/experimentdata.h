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
    explicit ExperimentData(QString experiment_name, QObject* parent = nullptr);

    const QString& experiment_name() const;

    void addExperimentRun(const AggServerResponse::ExperimentRun& exp_run);
    ExperimentRunData& getExperimentRun(quint32 exp_run_id) const;

    void updateData(quint32 exp_run_id, const AggServerResponse::ExperimentState& exp_state);

signals:
    void dataUpdated(quint32 exp_run_id, qint64 last_updated_time);

private slots:
    void experimentRunDataUpdated(qint64 last_updated_time);

private:
    QString experiment_name_;
    std::unordered_map<quint32, std::unique_ptr<ExperimentRunData>> experiment_run_map_;
};

}

#endif // EXPERIMENTDATA_H