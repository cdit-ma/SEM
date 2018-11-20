#ifndef WORKLOADEVENTSERIES_H
#define WORKLOADEVENTSERIES_H

#include "eventseries.h"

class WorkloadEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit WorkloadEventSeries(QString workerInstID, quint32 workloadID, QObject* parent = 0);

    const QString getWorkerInstanceID();
    const quint32 getWorkloadID();

private:
    QString workerInstanceID_;
    quint32 workloadID_;

};

#endif // WORKLOADEVENTSERIES_H
