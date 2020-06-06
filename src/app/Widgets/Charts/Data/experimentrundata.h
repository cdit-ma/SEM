#ifndef EXPERIMENTRUNDATA_H
#define EXPERIMENTRUNDATA_H

#include "nodedata.h"
#include "portconnectiondata.h"
#include "markersetdata.h"

namespace MEDEA {

class ExperimentRunData : public QObject
{
    Q_OBJECT

public:
    ExperimentRunData(QString exp_name,
                      quint32 experiment_run_id,
                      quint32 job_num,
                      qint64 start_time,
                      qint64 end_time = 0,
                      qint64 last_updated_time = 0,
                      QObject* parent = nullptr);

    const QString& experiment_name() const;

    quint32 experiment_run_id() const;
    quint32 job_num() const;

    qint64 start_time() const;
    qint64 end_time() const;
    qint64 last_updated_time() const;

    QList<NodeData*> getNodeData(const QString& hostname = "") const;
    QList<PortConnectionData*> getPortConnectionData(const QString& id = "") const;
    QList<MarkerSetData*> getMarkerSetData(const QString& id = "") const;

    QList<QPointer<const MEDEA::EventSeries>> getPortLifecycleSeries() const;
    QList<QPointer<const MEDEA::EventSeries>> getPortEventSeries() const;
    QList<QPointer<const MEDEA::EventSeries>> getWorkloadEventSeries() const;
    QList<QPointer<const MEDEA::EventSeries>> getCPUUtilisationSeries() const;
    QList<QPointer<const MEDEA::EventSeries>> getMemoryUtilisationSeries() const;
    QList<QPointer<const MEDEA::EventSeries>> getNetworkUtilisationSeries() const;
    //QList<QPointer<const MEDEA::EventSeries>> getMarkerEventSeries() const;

    void updateData(const AggServerResponse::ExperimentState& exp_state);

signals:
    void requestData(quint32 exp_run_id);
    void dataUpdated(qint64 last_updated_time);
    void experimentRunFinished(quint32 exp_run_id);

private:
    void addNodeData(const AggServerResponse::Node& node);
    void addPortConnection(const AggServerResponse::PortConnection& port_connection);
    void addMarkerSet(const QString& marker_name);

    QString experiment_name_;
    quint32 experiment_run_id_;
    quint32 job_num_;

    qint64 start_time_;
    qint64 end_time_;
    qint64 last_updated_time_;

    QHash<QString, NodeData*> node_data_hash_;
    QHash<QString, PortConnectionData*> port_connection_hash_;
    QHash<QString, MarkerSetData*> marker_set_hash_;
};

}

#endif // EXPERIMENTRUNDATA_H