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
WorkloadEvent::WorkloadEvent(WorkloadEvent::WorkerInstance inst, WorkloadEvent::WorkloadEventType type, quint32 workloadID, qint64 time, QString functionName, QString args, quint32 logLevel, QObject *parent)
    : MEDEA::Event(time, inst.name, parent)
{
    workerInstance_ = inst;
    type_ = type;
    workloadID_ = workloadID;
    functionName_ = functionName;
    args_ = args;
    logLevel_ = logLevel;
}


/**
 * @brief WorkloadEvent::getWorkloadPath
 * @return
 */
QString WorkloadEvent::getWorkloadPath() const
{
    return getWorkerInstanceGraphmlID() + "_" + getWorkloadID();
}


/**
 * @brief WorkloadEvent::getWorkerInstanceGraphmlID
 * @return
 */
QString WorkloadEvent::getWorkerInstanceGraphmlID() const
{
    return workerInstance_.graphml_id;
}


/**
 * @brief WorkloadEvent::getType
 * @return
 */
const WorkloadEvent::WorkloadEventType& WorkloadEvent::getType() const
{
    return type_;
}


/**
 * @brief WorkloadEvent::getWorkloadID
 * @return
 */
const quint32& WorkloadEvent::getWorkloadID() const
{
    return workloadID_;
}


/**
 * @brief WorkloadEvent::getLogLevel
 * @return
 */
const quint32& WorkloadEvent::getLogLevel() const
{
    return logLevel_;
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
 * @brief WorkloadEvent::getArgs
 * @return
 */
const QString& WorkloadEvent::getArgs() const
{
    return args_;
}
