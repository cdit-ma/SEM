#ifndef COMPONENTINSTANCEDATA_H
#define COMPONENTINSTANCEDATA_H

#include "protomessagestructs.h"
#include "portinstancedata.h"
#include "workerinstancedata.h"

class ComponentInstanceData {

public:
    ComponentInstanceData(const AggServerResponse::ComponentInstance& component_instance);

    const QString& getGraphmlID() const;
    const QString& getName() const;
    const QString& getPath() const;
    const QString& getType() const;

    void addPortInstanceData(const AggServerResponse::Port& port);
    QList<PortInstanceData> getPortInstanceData() const;

    void addWorkerInstanceData(const AggServerResponse::WorkerInstance& worker_instance);
    QList<WorkerInstanceData> getWorkerInstanceData() const;

private:
    QString graphml_id_;
    QString name_;
    QString path_;
    QString type_;

    QHash<QString, PortInstanceData> port_inst_data_hash_;
    QHash<QString, WorkerInstanceData> worker_inst_data_hash_;
};

#endif // COMPONENTINSTANCEDATA_H
