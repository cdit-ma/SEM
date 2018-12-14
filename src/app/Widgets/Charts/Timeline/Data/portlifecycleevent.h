<<<<<<< HEAD
#ifndef PORTLIFECYCLEEVENT_H
#define PORTLIFECYCLEEVENT_H

#include "../../Data/Events/event.h"

enum class LifecycleType {
=======
#ifndef PORTLIFECYCLE_H
#define PORTLIFECYCLE_H

#include <QObject>

enum class LifeCycleType {
>>>>>>> feature-hub
    NO_TYPE,
    CONFIGURE,
    ACTIVATE,
    PASSIVATE,
    TERMINATE
};

<<<<<<< HEAD
/*
struct Node {
    QString hostname;
    QString ip;
};

struct Component {
    QString nsme;
};

struct ComponentInstance {
    QString name;
    QString path;
    Component component;
    Node node;
};
*/

=======
>>>>>>> feature-hub
struct Port {
    enum Kind {
        NO_KIND,
        PERIODIC,
        PUBLISHER,
        SUBSCRIBER,
        REQUESTER,
        REPLIER
    };
<<<<<<< HEAD
    Kind kind;
    QString name;
    QString path;
    QString middleware;
    QString graphml_id;
};


class PortLifecycleEvent : public MEDEA::Event
=======
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
>>>>>>> feature-hub
{
    Q_OBJECT

public:
<<<<<<< HEAD
    explicit PortLifecycleEvent(Port port, LifecycleType type, qint64 time, QObject* parent = 0);

    const Port getPort();
    const LifecycleType getType();

    const QString getPortGraphmlID();
    const QString getPortPath();

private:
    Port port_;
    LifecycleType type_;

};

#endif // PORTLIFECYCLEEVENT_H
=======
    explicit PortLifeCycle(Port port, LifeCycleType type, qint64 time, QObject* parent = 0);
    explicit PortLifeCycle(LifeCycleType type, qint64 time, QObject* parent = 0);

    Port getPort();
    LifeCycleType getType();
    qint64 getTime();

private:
    Port port_;
    LifeCycleType type_;
    qint64 time_;

};

#endif // PORTLIFECYCLE_H
>>>>>>> feature-hub
