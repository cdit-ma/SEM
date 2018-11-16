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


class PortLifecycleEvent : public QObject
{
    Q_OBJECT

public:
    explicit PortLifecycleEvent(Port port, LifecycleType type, qint64 time, QObject* parent = 0);
    explicit PortLifecycleEvent(LifecycleType type, qint64 time, QObject* parent = 0);

    Port getPort();
    LifecycleType getType();
    qint64 getTime();

private:
    Port port_;
    LifecycleType type_;
    qint64 time_;

};

#endif // PORTLIFECYCLEEVENT_H
