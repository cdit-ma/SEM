#ifndef PORTLIFECYCLE_H
#define PORTLIFECYCLE_H

#include <QObject>

enum class LifeCycleType {
    NO_TYPE,
    CONFIGURE,
    ACTIVATE,
    PASSIVATE,
    TERMINATE
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
    int ID;
    Kind kind;
    QString path;
    QString name;
    QString middleware;
};

struct Component {
    int ID;
    QString nsme;
};

struct Node {
    int ID;
    QString hostname;
    QString ip;
};

struct ComponentInstance {
    int ID;
    Component component;
    Node node;
    QString name;
    QString path;
};


class PortLifeCycle : public QObject
{
    Q_OBJECT

public:
    explicit PortLifeCycle(Port port, LifeCycleType type, quint64 time, QObject* parent = 0);
    explicit PortLifeCycle(LifeCycleType type, quint64 time, QObject* parent = 0);

    Port getPort();
    LifeCycleType getType();
    quint64 getTime();

private:
    Port port_;
    LifeCycleType type_;
    quint64 time_;

};

#endif // PORTLIFECYCLE_H
