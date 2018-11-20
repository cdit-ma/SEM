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
    : MEDEA::Event(time, functionName, parent)
{
    workerInstance_ = inst;
    type_ = type;
    workloadID_ = workloadID;
    functionName_ = functionName;
    args_ = args;
    logLevel_ = logLevel;
}


/**
 * @brief WorkloadEvent::getWorkerInstanceGraphmlID
 * @return
 */
const QString WorkloadEvent::getWorkerInstanceGraphmlID()
{
    return workerInstance_.graphml_id;
}


/**
 * @brief WorkloadEvent::getType
 * @return
 */
const WorkloadEvent::WorkloadEventType WorkloadEvent::getType()
{
    return type_;
}


/**
 * @brief WorkloadEvent::getWorkloadID
 * @return
 */
const quint32 WorkloadEvent::getWorkloadID()
{
    return workloadID_;
}


/**
 * @brief WorkloadEvent::getLogLevel
 * @return
 */
const quint32 WorkloadEvent::getLogLevel()
{
    return logLevel_;
}
