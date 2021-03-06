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

    QString toString(const QString& dateTimeFormat) const override;

    const QString& getID() const override;

    AggServerResponse::LifecycleType getType() const;
    static const QString& getTypeString(AggServerResponse::LifecycleType type);

private:
    AggServerResponse::LifecycleType type_;

    QString id_;
};

#endif // PORTLIFECYCLEEVENT_H