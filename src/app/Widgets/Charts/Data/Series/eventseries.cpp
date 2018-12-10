#include "eventseries.h"

#include <QDateTime>
#include <QTextStream>

int MEDEA::EventSeries::eventSeries_ID = 0;

/**
 * @brief MEDEA::EventSeries::BaseSeries
 * @param parent
 */
MEDEA::EventSeries::EventSeries(QObject* parent, TIMELINE_SERIES_KIND kind)
    : QObject(parent)
{
    kind_ = kind;
    minTime_ = QDateTime::currentMSecsSinceEpoch();
    maxTime_ = 0;
    eventSeriesID_ = eventSeries_ID++;
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
 * @param time
 */
void MEDEA::EventSeries::addEvent(qint64 time)
{
    addEvent(new Event(time));
}


/**
 * @brief MEDEA::EventSeries::addEvent
 * @param event
 */
void MEDEA::EventSeries::addEvent(Event* event)
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
void MEDEA::EventSeries::addEvents(QList<Event*>& events)
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
qint64 MEDEA::EventSeries::getMinTimeMS() const
{
    return minTime_;
}


/**
 * @brief MEDEA::EventSeries::getMaxTimeMS
 * @return
 */
qint64 MEDEA::EventSeries::getMaxTimeMS() const
{
    return maxTime_;
}


/**
 * @brief MEDEA::EventSeries::getTimeRangeMS
 * @return
 */
QPair<qint64, qint64> MEDEA::EventSeries::getTimeRangeMS() const
{
    return {minTime_, maxTime_};
}


/**
 * @brief MEDEA::EventSeries::getKind
 * @return
 */
TIMELINE_SERIES_KIND MEDEA::EventSeries::getKind() const
{
    return kind_;
}


/**
 * @brief MEDEA::EventSeries::getID
 * @return
 */
QString MEDEA::EventSeries::getID() const
{
    return QString::number(eventSeriesID_);
}


/**
 * @brief MEDEA::EventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString MEDEA::EventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat)
{
    auto current = std::lower_bound(events_.cbegin(), events_.cend(), fromTimeMS, [](const MEDEA::Event* e, const qint64 &time) {
        return e->getTimeMS() < time;
    });
    auto upper = std::upper_bound(events_.cbegin(), events_.cend(), toTimeMS, [](const qint64 &time, const MEDEA::Event* e) {
        return time < e->getTimeMS();
    });

    int count = std::distance(current, upper);
    if (count <= 0)
        return "";

    QString hoveredData;
    QTextStream stream(&hoveredData);
    numberOfItemsToDisplay = qMin(count, numberOfItemsToDisplay);

    for (int i = 0; i < numberOfItemsToDisplay; i++) {
        auto event = (*current);
        stream << QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString(displayFormat) << "\n";
        current++;
    }
    if (count > numberOfItemsToDisplay)
        stream << "... (more omitted)";

    return hoveredData.trimmed();

    /*return "Hovered range: " +
            QDateTime::fromMSecsSinceEpoch(fromTimeMS).toString(displayFormat) +
            ", " +
            QDateTime::fromMSecsSinceEpoch(toTimeMS).toString(displayFormat);*/
}


/**
 * @brief MEDEA::EventSeries::getDefaultDisplayFormat
 * @return
 */
QString MEDEA::EventSeries::getDefaultDisplayFormat()
{
    return "MMM d, hh:mm:ss:zzz";
}


/**
 * @brief MEDEA::EventSeries::getDefaultNumberOfItemsToDisplay
 * @return
 */
int MEDEA::EventSeries::getDefaultNumberOfItemsToDisplay()
{
    return 10;
}

