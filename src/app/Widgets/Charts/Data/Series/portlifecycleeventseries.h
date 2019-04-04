#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "eventseries.h"

class PortLifecycleEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(const QString& ID, QObject* parent = 0);
};

#endif // PORTLIFECYCLEEVENTSERIES_H
