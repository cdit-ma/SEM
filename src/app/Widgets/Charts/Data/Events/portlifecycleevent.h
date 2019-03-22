#ifndef PORTLIFECYCLEEVENT_H
#define PORTLIFECYCLEEVENT_H

#include "event.h"

class PortLifecycleEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    enum PortKind {
        NO_KIND,
        PERIODIC,
        PUBLISHER,
        SUBSCRIBER,
        REQUESTER,
        REPLIER
    };

    explicit PortLifecycleEvent(Port port, PortKind kind, LifecycleType type, qint64 time, QObject* parent = 0);

    const PortKind& getPortKind() const;
    const Port& getPort() const;
    const LifecycleType& getType() const;

    QString getID() const;
    TIMELINE_DATA_KIND getKind() const;

private:    
    Port port_;
    PortKind kind_;
    LifecycleType type_;
};

#endif // PORTLIFECYCLEEVENT_H
