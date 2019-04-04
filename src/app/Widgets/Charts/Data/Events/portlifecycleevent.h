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

    explicit PortLifecycleEvent(const AggServerResponse::Port& port,
                                PortKind kind,
                                AggServerResponse::LifecycleType type,
                                qint64 time,
                                QObject* parent = 0);

    QString toString(const QString& dateTimeFormat) const;

    const QString& getID() const;
    const AggServerResponse::Port& getPort() const;

    PortKind getPortKind() const;
    AggServerResponse::LifecycleType getType() const;

private:    
    static const QString& getTypeString(AggServerResponse::LifecycleType type);

    const PortKind kind_;
    const AggServerResponse::Port port_;
    const AggServerResponse::LifecycleType type_;

};

#endif // PORTLIFECYCLEEVENT_H
