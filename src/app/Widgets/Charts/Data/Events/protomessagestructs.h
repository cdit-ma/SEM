#ifndef PROTOMESSAGESTRUCTS_H
#define PROTOMESSAGESTRUCTS_H

#include <QVector>
#include <QString>


namespace AggServerResponse{

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
    

    struct ExperimentRun {
        QString experiment_name;
        quint32 job_num;
        qint64 start_time;
        qint64 end_time;
        qint64 last_updated_time = 0;
        qint32 experiment_run_id = -1;
    };


    struct Experiment {
        QString name;
        QVector<ExperimentRun> runs;
    };
    
};

Q_DECLARE_METATYPE(AggServerResponse::ExperimentState);
Q_DECLARE_METATYPE(AggServerResponse::ExperimentRun);

#endif // PROTOMESSAGESTRUCTS_H
