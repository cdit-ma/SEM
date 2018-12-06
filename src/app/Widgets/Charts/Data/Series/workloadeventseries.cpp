#include "workloadeventseries.h"
#include "../Events/workloadevent.h"

#include <QDateTime>

/**
 * @brief WorkloadEventSeries::WorkloadEventSeries
 * @param workerInstID
 * @param workloadID
 * @param parent
 */
WorkloadEventSeries::WorkloadEventSeries(QString workerInstID, quint32 workloadID, QObject* parent)
    : MEDEA::EventSeries(parent, TIMELINE_SERIES_KIND::EVENT)
{
    workerInstanceID_ = workerInstID;
    workloadID_ = workloadID;
}


/**
 * @brief WorkloadEventSeries::getWorkerInstanceID
 * @return
 */
const QString& WorkloadEventSeries::getWorkerInstanceID() const
{
    return workerInstanceID_;
}


/**
 * @brief WorkloadEventSeries::getWorkloadID
 * @return
 */
const quint32& WorkloadEventSeries::getWorkloadID() const
{
    return workloadID_;
}


/**
 * @brief WorkloadEventSeries::getWorkloadPath
 * @return
 */
QString WorkloadEventSeries::getWorkloadPath() const
{
    return workerInstanceID_ + "_" + workloadID_;
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
        if (event) {
            return "[" + event->getFunctionName() + "]: "
                    //+ QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("MMM d, h:mm:ss:zzz A")
                    + QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("MMM d, hh:mm:ss:zzz")
                    + "\n" + event->getArgs();
        } else {
            return "";
        }
    } else {
        return QString::number(count);
    }
}
