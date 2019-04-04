#ifndef WORKLOADEVENTSERIES_H
#define WORKLOADEVENTSERIES_H

#include "eventseries.h"

class WorkloadEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit WorkloadEventSeries(const QString& ID, QObject* parent = 0);
};

#endif // WORKLOADEVENTSERIES_H
