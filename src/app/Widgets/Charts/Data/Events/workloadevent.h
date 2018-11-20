#ifndef WORKLOADEVENT_H
#define WORKLOADEVENT_H

#include "event.h"


class WorkloadEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    enum WorkloadEventType {
        STARTED,
        FINISHED,
        MESSAGE,
        WARNING,
        ERROR_EVENT
    };

    struct WorkerInstance {
        QString name;
        QString path;
        QString graphml_id;
    };

    explicit WorkloadEvent(WorkerInstance inst, WorkloadEventType type, quint32 workloadID, qint64 time, QString functionName, QString args, quint32 logLevel, QObject* parent = 0);

    const QString getWorkerInstanceGraphmlID();
    const WorkloadEventType getType();
    const quint32 getWorkloadID();
    const quint32 getLogLevel();

private:
    WorkerInstance workerInstance_;
    WorkloadEventType type_;
    quint32 workloadID_;

    QString functionName_;
    QString args_;
    quint32 logLevel_;

};

#endif
