#include "workloadeventseries.h"


/**
 * @brief WorkloadEventSeries::WorkloadEventSeries
 * @param workerInstID
 * @param workloadID
 * @param parent
 */
WorkloadEventSeries::WorkloadEventSeries(QString workerInstID, quint32 workloadID, QObject* parent)
    : MEDEA::EventSeries(parent)
{
    workerInstanceID_ = workerInstID;
    workloadID_ = workloadID;
}


/**
 * @brief WorkloadEventSeries::getWorkloadPath
 * @return
 */
const QString WorkloadEventSeries::getWorkloadPath()
{
    return workerInstanceID_ + "_" + workloadID_;
}


/**
 * @brief WorkloadEventSeries::getWorkerInstanceID
 * @return
 */
const QString WorkloadEventSeries::getWorkerInstanceID()
{
    return workerInstanceID_;
}


/**
 * @brief WorkloadEventSeries::getWorkloadID
 * @return
 */
const quint32 WorkloadEventSeries::getWorkloadID()
{
    return workloadID_;
}
