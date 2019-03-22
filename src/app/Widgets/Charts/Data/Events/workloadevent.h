#ifndef WORKLOADEVENT_H
#define WORKLOADEVENT_H

#include "event.h"


class WorkloadEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    enum WorkloadEventType {
        UNKNOWN,
        STARTED,
        FINISHED,
        MESSAGE,
        WARNING,
        ERROR_EVENT,
        MARKER
    };

    explicit WorkloadEvent(AggServerResponse::WorkerInstance inst, WorkloadEventType type, quint32 workloadID, qint64 time, QString functionName, QString args, quint32 logLevel, QObject* parent = 0);

    const WorkloadEventType& getType() const;
    const quint32& getWorkloadID() const;
    const quint32& getLogLevel() const;
    const QString& getFunctionName() const;
    const QString& getArgs() const;

    QString getID() const;
    TIMELINE_DATA_KIND getKind() const;

private:
    AggServerResponse::WorkerInstance workerInstance_;
    WorkloadEventType type_;
    quint32 workloadID_;

    QString functionName_;
    QString args_;
    quint32 logLevel_;

};

#endif // WORKLOADEVENT_H
