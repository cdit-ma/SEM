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
                           QString functionName,
                           QString args,
                           quint32 logLevel,
                           QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const override;

    const QString& getID() const override;

    const QString& getArgs() const;
    const QString& getFunctionName() const;
    const QString& getWorkerInstPath() const;

    WorkloadEventType getType() const;
    quint32 getWorkloadID() const;
    quint32 getLogLevel() const;

private:
    WorkloadEventType type_;

    quint32 workloadID_;
    quint32 logLevel_;

    QString id_;
    QString functionName_;
    QString args_;
    QString worker_inst_path_;
};

#endif // WORKLOADEVENT_H