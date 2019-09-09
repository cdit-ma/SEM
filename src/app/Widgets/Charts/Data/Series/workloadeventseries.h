#ifndef WORKLOADEVENTSERIES_H
#define WORKLOADEVENTSERIES_H

#include "eventseries.h"

class WorkloadEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit WorkloadEventSeries(const QString& ID, QObject* parent = nullptr);

    void addEvent(MEDEA::Event* event);
};

#endif // WORKLOADEVENTSERIES_H
