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

    QList<ComponentInstanceData*> getComponentInstanceData(const QString& path = "") const;
    QList<ComponentInstanceData*> getComponentInstanceData(const QStringList& comp_names, const QStringList& paths) const;

    QList<QPointer<const MEDEA::EventSeries>> getPortLifecycleSeries(const QString& port_path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getPortLifecycleSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const;

    QList<QPointer<const MEDEA::EventSeries>> getPortEventSeries(const QString& port_path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getPortEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const;

    QList<QPointer<const MEDEA::EventSeries>> getWorkloadEventSeries(const QString& worker_inst_path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getWorkloadEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& worker_inst_paths) const;

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
    QMultiHash<QString, QString> comp_name_hash_;
};

#endif // CONTAINERINSTANCEDATA_H