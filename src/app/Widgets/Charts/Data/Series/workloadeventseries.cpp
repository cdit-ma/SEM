#include "workloadeventseries.h"
#include "../Events/workloadevent.h"

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


/**
 * @brief WorkloadEventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param displayFormat
 * @return
 */
QString WorkloadEventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, QString displayFormat)
{
    const auto& data = getEvents();
    auto current = std::lower_bound(data.cbegin(), data.cend(), fromTimeMS, [](const MEDEA::Event* e, const qint64 &time) {
        return e->getTimeMS() < time;
    });
    auto upper = std::upper_bound(data.cbegin(), data.cend(), toTimeMS, [](const qint64 &time, const MEDEA::Event* e) {
        return time < e->getTimeMS();
    });

    int count = std::distance(current, upper);
    if (count <= 0) {
        return "";
    } else if (count == 1) {
        // args + function name
        auto event = (WorkloadEvent*)(*current);
        if (event)
            hovereData_ = "[" + event->getFunctionName() + "]\n" + event->getArgs();
        return hovereData_.trimmed();
    } else {
        return QString::number(count);
    }
}
