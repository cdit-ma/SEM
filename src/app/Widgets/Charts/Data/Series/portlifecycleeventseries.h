#ifndef PORTLIFECYCLEEVENTSERIES_H
#define PORTLIFECYCLEEVENTSERIES_H

#include "eventseries.h"
#include "../Events/portlifecycleevent.h"


class PortLifecycleEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit PortLifecycleEventSeries(QString path, QObject* parent = 0);

    QString getPortPath();

private:
    QString port_path;

};

#endif // PORTLIFECYCLEEVENTSERIES_H
