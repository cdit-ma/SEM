#ifndef WORKLOADEVENTSERIES_H
#define WORKLOADEVENTSERIES_H

#include "eventseries.h"
#include "../Events/workloadevent.h"

class WorkloadEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit WorkloadEventSeries(QString workloadPath, QObject* parent = 0);

    const QString& getWorkloadPath() const;

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

private:
    QString workloadPath_;

};

#endif // WORKLOADEVENTSERIES_H
