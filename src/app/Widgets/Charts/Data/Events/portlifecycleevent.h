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

    explicit PortLifecycleEvent(AggServerResponse::Port port, AggServerResponse::LifecycleType type, qint64 time, QObject* parent = 0);

    const AggServerResponse::PortKind& getPortKind() const;
    const AggServerResponse::Port& getPort() const;
    const AggServerResponse::LifecycleType& getType() const;

    QString getID() const;
    TIMELINE_DATA_KIND getKind() const;

private:    
    AggServerResponse::Port port_;
    AggServerResponse::PortKind kind_;
    AggServerResponse::LifecycleType type_;
};

#endif // PORTLIFECYCLEEVENT_H
