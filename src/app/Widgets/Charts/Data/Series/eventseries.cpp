#include "eventseries.h"

int MEDEA::EventSeries::eventSeries_ID = 0;

/**
 * @brief MEDEA::EventSeries::EventSeries
 * @param ID
 * @param kind
 * @param parent
 */
MEDEA::EventSeries::EventSeries(QString ID, TIMELINE_DATA_KIND kind, QObject* parent)
    : QObject(parent)
{
    ID_ = ID;
    kind_ = kind;
    eventSeriesID_ = eventSeries_ID++;

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
    // reset the time range
    minTime_ = QDateTime::currentMSecsSinceEpoch();
    maxTime_ = 0;
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
const QList<MEDEA::Event*> &MEDEA::EventSeries::getEvents()
{
    return events_;
}


/**
 * @brief MEDEA::EventSeries::getMinTimeMS
 * @return
 */
const qint64& MEDEA::EventSeries::getMinTimeMS() const
{
    return minTime_;
}


/**
 * @brief MEDEA::EventSeries::getMaxTimeMS
 * @return
 */
const qint64& MEDEA::EventSeries::getMaxTimeMS() const
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
 * @brief MEDEA::EventSeries::getEventSeriesID
 * @return
 */
QString MEDEA::EventSeries::getEventSeriesID() const
{
    return QString::number(eventSeriesID_);
}


/**
 * @brief MEDEA::EventSeries::getID
 * @return
 */
const QString& MEDEA::EventSeries::getID() const
{
    return ID_;
}


/**
 * @brief MEDEA::EventSeries::getKind
 * @return
 */
const TIMELINE_DATA_KIND& MEDEA::EventSeries::getKind() const
{
    return kind_;
}


/**
 * @brief MEDEA::EventSeries::getHoveredDataString
 * @param timeRangeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString MEDEA::EventSeries::getHoveredDataString(QPair<qint64, qint64> timeRangeMS, int numberOfItemsToDisplay, QString displayFormat)
{
    return getHoveredDataString(timeRangeMS.first, timeRangeMS.second, numberOfItemsToDisplay, displayFormat);
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
}


/**
 * @brief MEDEA::EventSeries::getDefaultDisplayFormat
 * @return
 */
QString MEDEA::EventSeries::getDefaultDisplayFormat()
{
    return "MMM d, hh:mm:ss.zzz";
}


/**
 * @brief MEDEA::EventSeries::getDefaultNumberOfItemsToDisplay
 * @return
 */
int MEDEA::EventSeries::getDefaultNumberOfItemsToDisplay()
{
    return 10;
}
