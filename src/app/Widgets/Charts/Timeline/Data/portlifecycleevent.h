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


class PortLifecycleEvent : public QObject
{
    Q_OBJECT

public:
    // time should be in milliseconds since epoch
    explicit PortLifecycleEvent(Port port, LifecycleType type, qint64 time, QObject* parent = 0);

    Port getPort();
    LifecycleType getType();
    qint64 getTime();

    const QString getPortPath();

private:
    Port port_;
    LifecycleType type_;
    qint64 time_;

};

#endif // PORTLIFECYCLEEVENT_H
