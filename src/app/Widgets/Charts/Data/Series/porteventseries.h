#ifndef PORTEVENTSERIES_H
#define PORTEVENTSERIES_H

#include "eventseries.h"
#include "../Events/portevent.h"

class PortEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit PortEventSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event);
};

#endif // PORTEVENTSERIES_H
