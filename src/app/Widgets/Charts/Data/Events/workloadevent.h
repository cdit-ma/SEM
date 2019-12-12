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

    const QString& getSeriesID() const;
    const QString& getID() const;

    const QString& getArgs() const;
    const QString& getFunctionName() const;

    WorkloadEventType getType() const;
    quint32 getWorkloadID() const;
    quint32 getLogLevel() const;

private:
    WorkloadEventType type_;

    quint32 workloadID_;
    quint32 logLevel_;

    // The series_id_ is formed by what is required to group events together into a series
    QString series_id_;
    QString id_;
    QString functionName_;
    QString args_;
};

#endif // WORKLOADEVENT_H
