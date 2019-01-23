#ifndef WORKLOADEVENTSERIES_H
#define WORKLOADEVENTSERIES_H

#include "eventseries.h"

class WorkloadEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit WorkloadEventSeries(QString ID, QObject* parent = 0);

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());
};

#endif // WORKLOADEVENTSERIES_H
