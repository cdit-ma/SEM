#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "eventseries.h"
#include "portlifecycleevent.h"


class PortLifecycleEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(QObject* parent = 0);

    int getID();

    void addPortEvent(PortLifecycleEvent* event);
    void addPortEvents(QList<PortLifecycleEvent*> events);

    const QList<PortLifecycleEvent*>& getConstPortEvents();

private:
    QList<PortLifecycleEvent*> portEvents_;
    int ID_;

    static int series_ID;

};

#endif // PORTLIFECYCLEEVENTSERIES_H
