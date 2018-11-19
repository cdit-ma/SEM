#include "eventseries.h"

#include <QDateTime>


/**
 * @brief MEDEA::EventSeries::BaseSeries
 * @param parent
 */
MEDEA::EventSeries::EventSeries(QObject* parent)
    : QObject(parent)
{
    minTime_ = QDateTime::currentMSecsSinceEpoch();
    maxTime_ = 0;
}


/**
 * @brief MEDEA::EventSeries::clear
 */
void MEDEA::EventSeries::clear()
{
    auto i = events_.begin();
    while (i != events_.end()) {
        (*i)->deleteLater();
        i = events_.erase(i);
    }
}


/**
 * @brief MEDEA::EventSeries::addEvent
 * @param event
 */
void MEDEA::EventSeries::addEvent(MEDEA::Event* event)
{
    if (event) {
        // update the range
        auto eventTime = event->getTimeMS();
        if (eventTime < minTime_)
            minTime_ = eventTime;
        if (eventTime > maxTime_)
            maxTime_ = eventTime;
        events_.append(event);
    }
}


/**
 * @brief MEDEA::EventSeries::addEvents
 * @param events
 */
void MEDEA::EventSeries::addEvents(QList<MEDEA::Event*>& events)
{
    for (auto event : events) {
        addEvent(event);
    }
}


/**
 * @brief MEDEA::EventSeries::getEvents
 * @return
 */
const QList<MEDEA::Event*>& MEDEA::EventSeries::getEvents()
{
    return events_;
}


/**
 * @brief MEDEA::EventSeries::getMinTimeMS
 * @return
 */
qint64 MEDEA::EventSeries::getMinTimeMS()
{
    return minTime_;
}


/**
 * @brief MEDEA::EventSeries::getMaxTimeMS
 * @return
 */
qint64 MEDEA::EventSeries::getMaxTimeMS()
{
    return maxTime_;
}


/**
 * @brief MEDEA::EventSeries::getTimeRangeMS
 * @return
 */
QPair<qint64, qint64> MEDEA::EventSeries::getTimeRangeMS()
{
    return {minTime_, maxTime_};
}


/**
 * @brief MEDEA::EventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @return
 */
QString MEDEA::EventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, QString displayFormat)
{
    return "Hovered range: " +
            QDateTime::fromMSecsSinceEpoch(fromTimeMS).toString(displayFormat) +
            ", " +
            QDateTime::fromMSecsSinceEpoch(toTimeMS).toString(displayFormat);
}

