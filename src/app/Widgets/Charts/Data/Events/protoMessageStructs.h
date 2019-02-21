#ifndef PROTOMESSAGESTRUCTS_H
#define PROTOMESSAGESTRUCTS_H

#include <QVector>
#include <QString>
#include <QStringList>


/*
 * REQUESTS
 */

struct Request {
    quint32 experiment_run_id;
    QVector<qint64> time_interval;
};


struct EventRequest : Request {
    QStringList component_instance_paths;
    QStringList component_names;
};


struct PortLifecycleRequest : EventRequest {
    PortLifecycleRequest(quint32 experimentRunID) {
        experiment_run_id = experimentRunID;
    }
    QStringList port_paths;
};


struct WorkloadRequest : EventRequest {
    WorkloadRequest(quint32 experimentRunID) {
        experiment_run_id = experimentRunID;
    }
    QStringList worker_paths;
};


struct CPUUtilisationRequest : Request {
    CPUUtilisationRequest(quint32 experimentRunID) {
        experiment_run_id = experimentRunID;
    }
    QStringList node_ids;
};


struct MemoryUtilisationRequest : Request {
    MemoryUtilisationRequest(quint32 experimentRunID) {
        experiment_run_id = experimentRunID;
    }
    QStringList node_ids;
};


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
    enum Kind {
        NO_KIND,
        PERIODIC,
        PUBLISHER,
        SUBSCRIBER,
        REQUESTER,
        REPLIER
    };
    Kind kind;
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
    quint32 experiment_run_id;
    QVector<Node> nodes;
    QVector<Component> components;
    QVector<Worker> workers;
};
Q_DECLARE_METATYPE(ExperimentState);


struct ExperimentRun {
    QString experiment_name;
    quint32 experiment_run_id;
    quint32 job_num;
    qint64 start_time;
    qint64 end_time;
};
Q_DECLARE_METATYPE(ExperimentRun);


struct Experiment {
    QString name;
    QVector<ExperimentRun> runs;
};


#endif // PROTOMESSAGESTRUCTS_H
