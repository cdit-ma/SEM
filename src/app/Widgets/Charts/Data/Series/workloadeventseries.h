#ifndef WORKLOADEVENTSERIES_H
#define WORKLOADEVENTSERIES_H

#include "eventseries.h"

class WorkloadEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit WorkloadEventSeries(QString workerInstID, quint32 workloadID, QObject* parent = 0);

    const QString& getWorkerInstanceID() const;
    const quint32& getWorkloadID() const;

    QString getWorkloadPath() const;

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

private:
    QString workerInstanceID_;
    quint32 workloadID_;

};

#endif // WORKLOADEVENTSERIES_H
