#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "eventseries.h"
#include "../Events/portlifecycleevent.h"

class PortLifecycleEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event);
};

#endif // PORTLIFECYCLEEVENTSERIES_H
