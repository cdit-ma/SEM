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

    const QString& getSeriesID() const;
    const QString& getID() const;

    AggServerResponse::LifecycleType getType() const;
    static const QString& getTypeString(AggServerResponse::LifecycleType type);

private:
    AggServerResponse::LifecycleType type_;

    // The series_id_ is formed by what is required to group events together into a series
    QString series_id_;
    QString id_;
};

#endif // PORTLIFECYCLEEVENT_H
