#include "workloadevent.h"

/**
 * @brief WorkloadEvent::WorkloadEvent
 * @param inst
 * @param type
 * @param workloadID
 * @param time
 * @param functionName
 * @param args
 * @param logLevel
 * @param parent
 */
WorkloadEvent::WorkloadEvent(const AggServerResponse::WorkerInstance& inst,
                             WorkloadEvent::WorkloadEventType type,
                             quint32 workloadID,
                             qint64 time,
                             QString functionName,
                             QString args,
                             quint32 logLevel,
                             QObject *parent)
	: MEDEA::Event(MEDEA::ChartDataKind::WORKLOAD, time, parent),
	  type_(type),
	  workloadID_(workloadID),
	  logLevel_(logLevel),
	  id_(inst.graphml_id + QString::number(workloadID) + QString::number(time)),
	  functionName_(std::move(functionName)),
	  args_(std::move(args)),
      worker_inst_path_(inst.path) {}

/**
 * @brief WorkloadEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString WorkloadEvent::toString(const QString &dateTimeFormat) const
{
    QString dataString = "[" + functionName_ + "] - " + getDateTimeString(dateTimeFormat) + "\n";
    if (!args_.isEmpty()) {
        dataString += args_.trimmed() + "\n";
    }
    dataString += "\n";
    return dataString;
}

/**
 * @brief WorkloadEvent::getID
 * @return
 */
const QString& WorkloadEvent::getID() const
{
    return id_;
}

/**
 * @brief WorkloadEvent::getArgs
 * @return
 */
const QString& WorkloadEvent::getArgs() const
{
    return args_;
}

/**
 * @brief WorkloadEvent::getFunctionName
 * @return
 */
const QString& WorkloadEvent::getFunctionName() const
{
    return functionName_;
}

/**
 * @brief WorkloadEvent::getWorkerInstPath
 * @return
 */
const QString& WorkloadEvent::getWorkerInstPath() const
{
    return worker_inst_path_;
}

/**
 * @brief WorkloadEvent::getType
 * @return
 */
WorkloadEvent::WorkloadEventType WorkloadEvent::getType() const
{
    return type_;
}

/**
 * @brief WorkloadEvent::getWorkloadID
 * @return
 */
quint32 WorkloadEvent::getWorkloadID() const
{
    return workloadID_;
}

/**
 * @brief WorkloadEvent::getLogLevel
 * @return
 */
quint32 WorkloadEvent::getLogLevel() const
{
    return logLevel_;
}