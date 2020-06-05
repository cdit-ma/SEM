#ifndef NODEDATA_H
#define NODEDATA_H

#include "protomessagestructs.h"
#include "containerinstancedata.h"

#include "Requests/utilisationrequest.h"

#include "Events/cpuutilisationevent.h"
#include "Events/memoryutilisationevent.h"
#include "Events/networkutilisationevent.h"
#include "Series/cpuutilisationeventseries.h"
#include "Series/memoryutilisationeventseries.h"
#include "Series/networkutilisationeventseries.h"
#include <QSet>

class NodeData : public QObject
{
    Q_OBJECT

public:
    NodeData(quint32 exp_run_id, const AggServerResponse::Node& node, QObject* parent = nullptr);

    const QString& getHostname() const;
    const QString& getIP() const;

    QList<ContainerInstanceData*> getContainerInstanceData() const;

    const QSet<QPointer<const MEDEA::EventSeries>>& getPortLifecycleSeries() const;
    const QSet<QPointer<const MEDEA::EventSeries>>& getPortEventSeries() const;
    const QSet<QPointer<const MEDEA::EventSeries>>& getWorkloadEventSeries() const;

    const UtilisationRequest& getCPUUtilisationRequest() const;
    const UtilisationRequest& getMemoryUtilisationRequest() const;
    const UtilisationRequest& getNetworkUtilisationRequest() const;

    void addCPUUtilisationEvents(const QVector<CPUUtilisationEvent*>& events);
    QPointer<const MEDEA::EventSeries> getCPUUtilisationSeries() const;

    void addMemoryUtilisationEvents(const QVector<MemoryUtilisationEvent*>& events);
    QPointer<const MEDEA::EventSeries> getMemoryUtilisationSeries() const;

    void addNetworkUtilisationEvents(const QVector<NetworkUtilisationEvent*>& events);
    QPointer<const MEDEA::EventSeries> getNetworkUtilisationSeries() const;

    void updateData(const AggServerResponse::Node& node, qint64 new_last_updated_time);

signals:
    void requestData(NodeData& node);

private:
    void addContainerInstanceData(const AggServerResponse::Container& container);

    quint32 experiment_run_id_;
    qint64 last_updated_time_;

    QString hostname_;
    QString ip_;

    QHash<QString, ContainerInstanceData*> container_inst_data_hash_;

    QSet<QPointer<const MEDEA::EventSeries>> port_lifecycle_series_;
    QSet<QPointer<const MEDEA::EventSeries>> port_event_series_;
    QSet<QPointer<const MEDEA::EventSeries>> workload_event_series_;

    UtilisationRequest cpu_utilisation_request_;
    UtilisationRequest memory_utilisation_request_;
    UtilisationRequest network_utilisation_request_;

    CPUUtilisationEventSeries* cpu_utilisation_series_ = nullptr;
    MemoryUtilisationEventSeries* memory_utilisation_series_ = nullptr;
    NetworkUtilisationEventSeries* network_utilisation_series_ = nullptr;
};

#endif // NODEDATA_H