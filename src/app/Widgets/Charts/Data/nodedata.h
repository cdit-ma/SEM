#ifndef NODEDATA_H
#define NODEDATA_H

#include "protomessagestructs.h"
#include "containerinstancedata.h"

#include "Requests/cpuutilisationrequest.h"
#include "Requests/memoryutilisationrequest.h"

#include "Events/cpuutilisationevent.h"
#include "Events/memoryutilisationevent.h"
#include "Series/cpuutilisationeventseries.h"
#include "Series/memoryutilisationeventseries.h"

class NodeData : public QObject
{
    Q_OBJECT

public:
    NodeData(quint32 exp_run_id, const AggServerResponse::Node& node, QObject* parent = nullptr);

    const QString& getHostname() const;
    const QString& getIP() const;

    QList<ContainerInstanceData*> getContainerInstanceData() const;

    const CPUUtilisationRequest& getCPUUtilisationRequest() const;
    const MemoryUtilisationRequest& getMemoryUtilisationRequest() const;

    void addCPUUtilisationEvents(const QVector<CPUUtilisationEvent*>& events);
    CPUUtilisationEventSeries* getCPUUtilisationSeries() const;

    void addMemoryUtilisationEvents(const QVector<MemoryUtilisationEvent*>& events);
    MemoryUtilisationEventSeries* getMemoryUtilisationSeries() const;

    void updateData(const AggServerResponse::Node& node, qint64 last_updated_time);

signals:
    void requestData(NodeData& node);

private:
    void addContainerInstanceData(const AggServerResponse::Container& container);

    quint32 experiment_run_id_;
    qint64 last_updated_time_;

    QString hostname_;
    QString ip_;

    QHash<QString, ContainerInstanceData*> container_inst_data_hash_;

    CPUUtilisationRequest cpu_utilisation_request_;
    MemoryUtilisationRequest memory_utilisation_request_;

    CPUUtilisationEventSeries* cpu_utilisation_series_ = nullptr;
    MemoryUtilisationEventSeries* memory_utilisation_series_ = nullptr;
};

#endif // NODEDATA_H
