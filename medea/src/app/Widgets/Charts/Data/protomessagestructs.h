#ifndef PROTOMESSAGESTRUCTS_H
#define PROTOMESSAGESTRUCTS_H

#include <QVector>
#include <QString>
#include <QMetaType>

namespace AggServerResponse {

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

struct PortConnection {
    enum ConnectionType {
        PUBSUB,
        REQREP
    };
    ConnectionType type;
    QString from_port_graphml;
    QString to_port_graphml;
};

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

struct NetworkInterface {
    QString name;
    QString type; // eg "Ethernet" or "Local Loopback"
    QString description;
    QString ipv4;
    QString ipv6;
    QString mac_address;
    qint64 speed;
};

struct GPUDevice {
    QString name;
    QString vendor;
    qint32 gpu_device_index;
};

struct Node {
    QString hostname;
    QString ip;
    QVector<Container> containers;
    QVector<NetworkInterface> interfaces;
    QVector<GPUDevice> gpus;
};

struct ExperimentState {
    QVector<Node> nodes;
    QVector<Component> components;
    QVector<Worker> workers;
    QVector<PortConnection> port_connections;
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