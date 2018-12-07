#ifndef PORTLIFECYCLEEVENT_H
#define PORTLIFECYCLEEVENT_H

#include "event.h"

enum class LifecycleType {
    NO_TYPE,
    CONFIGURE,
    ACTIVATE,
    PASSIVATE,
    TERMINATE
};

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


class PortLifecycleEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit PortLifecycleEvent(Port port, LifecycleType type, qint64 time, QObject* parent = 0);

    const Port& getPort() const;
    const LifecycleType& getType() const;

    QString getPortGraphmlID() const;
    QString getPortPath() const;

private:    
    Port port_;
    LifecycleType type_;

};

#endif // PORTLIFECYCLEEVENT_H
