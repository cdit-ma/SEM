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

    explicit WorkloadEvent(const AggServerResponse::WorkerInstance& inst,
                           WorkloadEventType type,
                           quint32 workloadID,
                           qint64 time,
                           const QString& functionName,
                           const QString& args,
                           quint32 logLevel,
                           QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const;

    const QString& getID() const;
    const QString& getArgs() const;
    const QString& getFunctionName() const;

    WorkloadEventType getType() const;
    quint32 getWorkloadID() const;
    quint32 getLogLevel() const;

private:
    //const AggServerResponse::WorkerInstance& workerInstance_;
    const AggServerResponse::WorkerInstance workerInstance_;
    const WorkloadEventType type_;

    const quint32 workloadID_;
    const quint32 logLevel_;

    const QString functionName_;
    const QString args_;
    const QString ID_;

};

#endif // WORKLOADEVENT_H
