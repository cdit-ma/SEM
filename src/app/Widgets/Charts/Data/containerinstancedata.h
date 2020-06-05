#ifndef CONTAINERINSTANCEDATA_H
#define CONTAINERINSTANCEDATA_H

#include "protomessagestructs.h"
#include "componentinstancedata.h"
#include <QSet>

class ContainerInstanceData : public QObject
{
    Q_OBJECT

public:
    ContainerInstanceData(quint32 exp_run_id, const AggServerResponse::Container& container, QObject* parent = nullptr);

    const QString& getGraphmlID() const;
    const QString& getName() const;
    AggServerResponse::Container::ContainerType getType() const;

    QList<ComponentInstanceData*> getComponentInstanceData() const;

    const QSet<QPointer<const MEDEA::EventSeries>>& getPortLifecycleSeries() const;
    const QSet<QPointer<const MEDEA::EventSeries>>& getPortEventSeries() const;
    const QSet<QPointer<const MEDEA::EventSeries>>& getWorkloadEventSeries() const;

    void updateData(const AggServerResponse::Container& container, qint64 new_last_updated_time);

signals:
    void dataChanged(qint64 last_updated_time);

private:
    void addComponentInstanceData(const AggServerResponse::ComponentInstance& comp_inst);

    quint32 experiment_run_id_;
    qint64 last_updated_time_;

    QString graphml_id_;
    QString name_;

    AggServerResponse::Container::ContainerType type_;
    QHash<QString, ComponentInstanceData*> comp_inst_data_hash_;

    QSet<QPointer<const MEDEA::EventSeries>> port_lifecycle_series_;
    QSet<QPointer<const MEDEA::EventSeries>> port_event_series_;
    QSet<QPointer<const MEDEA::EventSeries>> workload_event_series_;
};

#endif // CONTAINERINSTANCEDATA_H