#ifndef MARKEREVENT_H
#define MARKEREVENT_H

#include "event.h"

class MarkerEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit MarkerEvent(const QString& markerName,
                         qint64 markerID,
                         const AggServerResponse::ComponentInstance& inst,
                         qint64 time,
                         QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const;

    const QString& getSeriesID() const;
    const QString& getID() const;

    qint64 getMarkerID() const;

private:
    QString series_id_;
    QString id_;

    qint64 markerID_;
};

#endif // MARKEREVENT_H
