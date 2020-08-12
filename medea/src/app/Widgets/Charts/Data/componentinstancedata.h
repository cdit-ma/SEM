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

    QList<PortInstanceData*> getPortInstanceData(const QString& path = "") const;
    QList<PortInstanceData*> getPortInstanceData(const QStringList& paths) const;

    QList<WorkerInstanceData*> getWorkerInstanceData(const QString& path = "") const;
    QList<WorkerInstanceData*> getWorkerInstanceData(const QStringList& paths) const;

    QList<QPointer<const MEDEA::EventSeries>> getPortLifecycleSeries(const QString& path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getPortLifecycleSeries(const QStringList& paths) const;

    QList<QPointer<const MEDEA::EventSeries>> getPortEventSeries(const QString& path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getPortEventSeries(const QStringList& paths) const;

    QList<QPointer<const MEDEA::EventSeries>> getWorkloadEventSeries(const QString& path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getWorkloadEventSeries(const QStringList& paths) const;

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

    // These data classes are hashed by their paths
    QHash<QString, PortInstanceData*> port_inst_data_hash_;
    QHash<QString, WorkerInstanceData*> worker_inst_data_hash_;
};

#endif // COMPONENTINSTANCEDATA_H