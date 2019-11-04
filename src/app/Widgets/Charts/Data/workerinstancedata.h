#ifndef WORKERINSTANCEDATA_H
#define WORKERINSTANCEDATA_H

#include "protomessagestructs.h"
#include "Events/workloadevent.h"
#include "Series/workloadeventseries.h"

class WorkerInstanceData {

public:
    WorkerInstanceData(const AggServerResponse::WorkerInstance& worker_instance);

    const QString& getGraphmlID() const;
    const QString& getName() const;
    const QString& getPath() const;
    const QString& getType() const;

    void addWorkloadEvents(const QVector<WorkloadEvent*>& events);
    WorkloadEventSeries* getWorkloadEventSeries() const;

private:
    QString graphml_id_;
    QString name_;
    QString path_;
    QString type_;

    WorkloadEventSeries* workload_event_series_ = nullptr;
};

#endif // WORKERINSTANCEDATA_H
