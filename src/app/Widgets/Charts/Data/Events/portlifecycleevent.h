#ifndef PORTLIFECYCLEEVENT_H
#define PORTLIFECYCLEEVENT_H

#include "event.h"

class PortLifecycleEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit PortLifecycleEvent(const AggServerResponse::Port& port,
                                AggServerResponse::LifecycleType type,
                                qint64 time,
                                QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const;

    const QString& getID() const;
    const AggServerResponse::Port& getPort() const;

    AggServerResponse::LifecycleType getType() const;
    static const QString& getTypeString(AggServerResponse::LifecycleType type);

private:
    AggServerResponse::Port port_;
    AggServerResponse::LifecycleType type_;
};

#endif // PORTLIFECYCLEEVENT_H
