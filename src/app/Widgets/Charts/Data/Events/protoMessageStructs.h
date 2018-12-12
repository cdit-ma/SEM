#ifndef PROTOMESSAGESTRUCTS_H
#define PROTOMESSAGESTRUCTS_H

#include <QString>
#include <QVector>


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
    ComponentInstance component_instances;
};


struct Node {
    QString hostname;
    QString ip;
    Container container;
};


struct ExperimentRun {
    QString experiment_name;
    quint32 experiment_run_id;
    quint32 job_num;
    qint64 start_time;
    qint64 end_time;
};


struct Experiment {
    QString name;
    QVector<ExperimentRun> runs;
};



#endif // PROTOMESSAGESTRUCTS_H
