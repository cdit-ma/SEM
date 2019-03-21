#ifndef MARKEREVENT_H
#define MARKEREVENT_H

#include "event.h"


class MarkerEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    explicit MarkerEvent(QString markerName, qint64 markerID, ComponentInstance inst, qint64 time, QObject* parent = 0);

    qint64 getMarkerID() const;

    QString getID() const;
    TIMELINE_DATA_KIND getKind() const;

private:
    ComponentInstance componentInstance_;
    qint64 markerID_;

};

#endif // MARKEREVENT_H
