#include "portlifecycleeventseries.h"

#include <QDateTime>

int PortLifecycleEventSeries::series_ID = 0;

/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(QObject* parent)
    : QObject(parent)
{
    ID_ = series_ID++;

    minTime_ = QDateTime::currentMSecsSinceEpoch() + 1E6;
    maxTime_ = 0;
}


/**
 * @brief PortLifecycleEventSeries::getID
 * @return
 */
int PortLifecycleEventSeries::getID()
{
    return -1; //ID_;
}


/**
 * @brief PortLifecycleEventSeries::addPortEvent
 * @param event
 */
void PortLifecycleEventSeries::addPortEvent(PortLifecycleEvent* event)
{
    if (event) {
        portEvents_.append(event);

        // update the range
        auto eventTime = event->getTime();
        if (eventTime < minTime_)
            minTime_ = eventTime;
        if (eventTime > maxTime_)
            maxTime_ = eventTime;
    }
}


/**
 * @brief PortLifecycleEventSeries::addPortEvents
 * @param events
 */
void PortLifecycleEventSeries::addPortEvents(QList<PortLifecycleEvent*>& events)
{
    //portEvents_.append(events);

    for (auto event : events) {
        addPortEvent(event);
    }
}


/**
 * @brief PortLifecycleEventSeries::getConstPortEvents
 * @return
 */
const QList<PortLifecycleEvent*>& PortLifecycleEventSeries::getConstPortEvents()
{
    return portEvents_;
}


/**
 * @brief PortLifecycleEventSeries::getRange
 * @return
 */
QPair<quint64, quint64> PortLifecycleEventSeries::getRange()
{
    return {minTime_, maxTime_};
}

