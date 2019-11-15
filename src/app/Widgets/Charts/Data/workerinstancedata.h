#ifndef WORKERINSTANCEDATA_H
#define WORKERINSTANCEDATA_H

#include "protomessagestructs.h"
#include "Requests/workloadrequest.h"
#include "Events/workloadevent.h"
#include "Series/workloadeventseries.h"

class ComponentInstanceData;
class WorkerInstanceData : public QObject
{
    Q_OBJECT

public:
    WorkerInstanceData(quint32 exp_run_id, const ComponentInstanceData& comp_inst, const AggServerResponse::WorkerInstance& worker_instance, QObject* parent = nullptr);

    const QString& getGraphmlID() const;
    const QString& getName() const;
    const QString& getPath() const;
    const QString& getType() const;

    const WorkloadRequest& getWorkloadRequest() const;

    void addWorkloadEvents(const QVector<WorkloadEvent*>& events);
    WorkloadEventSeries* getWorkloadEventSeries() const;

    void updateData(qint64 new_last_updated_time);

signals:
    void requestData(WorkerInstanceData& worker_inst);

private:
    quint32 experiment_run_id_;
    qint64 last_updated_time_;

    QString graphml_id_;
    QString name_;
    QString path_;
    QString type_;

    WorkloadRequest workload_request_;
    WorkloadEventSeries* workload_event_series_ = nullptr;
};

#endif // WORKERINSTANCEDATA_H
