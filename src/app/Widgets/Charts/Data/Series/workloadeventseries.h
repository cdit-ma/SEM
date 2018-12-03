#ifndef WORKLOADEVENTSERIES_H
#define WORKLOADEVENTSERIES_H

#include "eventseries.h"

class WorkloadEventSeries : public MEDEA::EventSeries
{
    Q_OBJECT

public:
    explicit WorkloadEventSeries(QString workerInstID, quint32 workloadID, QObject* parent = 0);

    const QString getWorkloadPath();

    const QString getWorkerInstanceID();
    const quint32 getWorkloadID();

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, QString displayFormat);

private:
    QString workerInstanceID_;
    quint32 workloadID_;

    QString hovereData_;
};

#endif // WORKLOADEVENTSERIES_H
