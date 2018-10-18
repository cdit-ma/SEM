#ifndef PORTLIFECYCLEEVENT_H
#define PORTLIFECYCLEEVENT_H

#include <QObject>

enum class LifecycleType {
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
    //int ID;
    QString path;
    //ComponentInstance component_instance;
    QString name;
    //Kind kind;
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


class PortLifecycleEvent : public QObject
{
    Q_OBJECT

public:
    explicit PortLifecycleEvent(Port port, LifecycleType type, quint64 time, QObject* parent = 0);
    explicit PortLifecycleEvent(LifecycleType type, quint64 time, QObject* parent = 0);

    Port getPort();
    LifecycleType getType();
    quint64 getTime();

private:
    Port port_;
    LifecycleType type_;
    quint64 time_;

};

#endif // PORTLIFECYCLEEVENT_H
