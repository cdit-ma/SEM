#ifndef COMPONENTINSTANCEDATA_H
#define COMPONENTINSTANCEDATA_H

#include "protomessagestructs.h"
#include "portinstancedata.h"
#include "workerinstancedata.h"

#include <QHash>

class ComponentInstanceData : public QObject
{
    Q_OBJECT

public:
    ComponentInstanceData(quint32 exp_run_id, const AggServerResponse::ComponentInstance& component_instance, QObject* parent = nullptr);

    const QString& getGraphmlID() const;
    const QString& getName() const;
    const QString& getPath() const;
    const QString& getType() const;

    QList<PortInstanceData*> getPortInstanceData() const;
    QList<WorkerInstanceData*> getWorkerInstanceData() const;

    QList<QPointer<const MEDEA::EventSeries>> getPortLifecycleSeries() const;
    QList<QPointer<const MEDEA::EventSeries>> getPortEventSeries() const;
    QList<QPointer<const MEDEA::EventSeries>> getWorkloadEventSeries() const;

    void updateData(const AggServerResponse::ComponentInstance& component_instance, qint64 new_last_updated_time);

signals:
    void dataChanged(qint64 last_updated_time);

private:
    void addPortInstanceData(const AggServerResponse::Port& port);
    void addWorkerInstanceData(const AggServerResponse::WorkerInstance& worker_instance);

    quint32 experiment_run_id_;
    qint64 last_updated_time_;

    QString graphml_id_;
    QString name_;
    QString path_;
    QString type_;

    QHash<QString, PortInstanceData*> port_inst_data_hash_;
    QHash<QString, WorkerInstanceData*> worker_inst_data_hash_;
};

#endif // COMPONENTINSTANCEDATA_H