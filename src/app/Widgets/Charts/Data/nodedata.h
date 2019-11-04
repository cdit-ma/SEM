#ifndef NODEDATA_H
#define NODEDATA_H

#include "protomessagestructs.h"
#include "Events/cpuutilisationevent.h"
#include "Events/memoryutilisationevent.h"
#include "Series/cpuutilisationeventseries.h"
#include "Series/memoryutilisationeventseries.h"

#include "containerinstancedata.h"

class NodeData {

public:
    NodeData(const AggServerResponse::Node& node);

    const QString& getHostname() const;
    const QString& getIP() const;

    void addContainerInstanceData(const AggServerResponse::Container& container);
    QList<ContainerInstanceData> getContainerInstanceData() const;

    void addCPUUtilisationEvents(const QVector<CPUUtilisationEvent*>& events);
    CPUUtilisationEventSeries* getCPUUtilisationSeries() const;

    void addMemoryUtilisationEvents(const QVector<MemoryUtilisationEvent*>& events);
    MemoryUtilisationEventSeries* getMemoryUtilisationSeries() const;

private:
    QString hostname_;
    QString ip_;

    QHash<QString, ContainerInstanceData> container_inst_data_hash_;

    CPUUtilisationEventSeries* cpu_utilisation_series_ = nullptr;
    MemoryUtilisationEventSeries* memory_utilisation_series_ = nullptr;
};

#endif // NODEDATA_H
