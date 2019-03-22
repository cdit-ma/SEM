#ifndef PROTOMESSAGESTRUCTS_H
#define PROTOMESSAGESTRUCTS_H

#include <QVector>
#include <QString>
#include <QStringList>


/*
 * REQUESTS
 */

/*
struct Request {
    quint32 experiment_run_id;
    QVector<qint64> time_interval;
    QStringList graphml_ids;
};


struct EventRequest : Request {
    QStringList component_instance_paths;
    QStringList component_instance_ids;
    QStringList component_names;
    QStringList paths;
};


struct UtilisationRequest : Request {
    QStringList node_hostnames;
};


struct PortLifecycleRequest : EventRequest {
    PortLifecycleRequest(quint32 experimentRunID) { experiment_run_id = experimentRunID; }
};


struct WorkloadRequest : EventRequest {
    WorkloadRequest(quint32 experimentRunID) { experiment_run_id = experimentRunID; }
};


struct CPUUtilisationRequest : UtilisationRequest {
    CPUUtilisationRequest(quint32 experimentRunID) { experiment_run_id = experimentRunID; }
};


struct MemoryUtilisationRequest : UtilisationRequest {
    MemoryUtilisationRequest(quint32 experimentRunID) { experiment_run_id = experimentRunID; }
};
*/

/*
 * ENUMS
 */

enum class LifecycleType {
    NO_TYPE,
    CONFIGURE,
    ACTIVATE,
    PASSIVATE,
    TERMINATE
};


/*
 * STRUCTS
 */

struct Port {
    QString name;
    QString path;
    QString middleware;
    QString graphml_id;
};


struct Worker {
    QString name;
};


struct Component {
    QString name;
};


struct WorkerInstance {
    QString name;
    QString path;
    QString graphml_id;
    QString type;
};


struct ComponentInstance {
    QString name;
    QString path;
    QString graphml_id;
    QString type;
    QVector<Port> ports;
    QVector<WorkerInstance> worker_instances;
};


struct Container {
    enum ContainerType {
        GENERIC,
        DOCKER
    };
    QString name;
    ContainerType type;
    QString graphml_id;
    QVector<ComponentInstance> component_instances;
};


struct Node {
    QString hostname;
    QString ip;
    QVector<Container> containers;
};


struct ExperimentState {
    QVector<Node> nodes;
    QVector<Component> components;
    QVector<Worker> workers;
    quint32 experiment_run_id;
    qint64 end_time;
    qint64 last_updated_time = 0;
};
Q_DECLARE_METATYPE(ExperimentState);


struct ExperimentRun {
    QString experiment_name;
    quint32 job_num;
    qint64 start_time;
    qint64 end_time;
    qint64 last_updated_time = 0;
    qint32 experiment_run_id = -1;
};
Q_DECLARE_METATYPE(ExperimentRun);


struct Experiment {
    QString name;
    QVector<ExperimentRun> runs;
};


#endif // PROTOMESSAGESTRUCTS_H
