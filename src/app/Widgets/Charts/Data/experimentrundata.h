#ifndef EXPERIMENTRUNDATA_H
#define EXPERIMENTRUNDATA_H

#include "nodedata.h"
#include "containerinstancedata.h"
#include "componentinstancedata.h"
#include "portinstancedata.h"
#include "workerinstancedata.h"
#include "portconnectiondata.h"
#include "markersetdata.h"

#include <QObject>

namespace MEDEA {

class ExperimentRunData : public QObject
{
    Q_OBJECT

public:
    ExperimentRunData(quint32 experiment_run_id,
                      quint32 job_num,
                      qint64 start_time,
                      qint64 end_time = 0,
                      qint64 last_updated_time = 0);

    quint32 experiment_run_id() const;
    quint32 job_num() const;

    qint64 start_time() const;
    qint64 end_time() const;
    qint64 last_updated_time() const;

    bool hasState() const;
    void setExperimentState(const AggServerResponse::ExperimentState& exp_state);

    void updateEndTime(qint64 time);
    void updateLastUpdatedTime(qint64 time);

    QList<NodeData> getNodeData(const QString& hostname = "") const;
    QList<PortConnectionData> getPortConnectionData(int id = -1) const;
    QList<MarkerSetData> getMarkerSetData(int id = -1) const;

signals:
    void dataChanged();

private:
    void addNodeData(const AggServerResponse::Node& node);
    void addPortConnection(const AggServerResponse::PortConnection& port_connection);
    void addMarkerSet(const QString& marker_name);

    void clearState();

    // TODO? - Implement helper functions for getting specific series (include filters)

    quint32 experiment_run_id_;
    quint32 job_num_;

    qint64 start_time_;
    qint64 end_time_;
    qint64 last_updated_time_;

    bool has_state_ = false;

    QHash<QString, NodeData> node_data_hash_;
    QHash<int, PortConnectionData> port_connection_hash_;
    QHash<int, MarkerSetData> marker_set_hash_;
};

}

#endif // EXPERIMENTRUNDATA_H
