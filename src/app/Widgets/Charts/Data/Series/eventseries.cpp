#include "eventseries.h"

#include <QTextStream>
#include <QDateTime>

std::atomic<int> MEDEA::EventSeries::eventSeries_ID(0);

/**
 * @brief MEDEA::EventSeries::EventSeries
 * @param ID
 * @param kind
 * @param parent
 */
MEDEA::EventSeries::EventSeries(const QString& ID, MEDEA::ChartDataKind kind, QObject* parent)
	: QObject(parent),
	  ID_(ID),
	  label_(ID),
	  kind_(kind),
	  eventSeriesID_(eventSeries_ID++),
	  eventSeriesIDStr_(QString::number(eventSeriesID_))
{
    minTime_ = QDateTime::currentMSecsSinceEpoch();
    maxTime_ = 0;
}

/**
 * @brief MEDEA::EventSeries::~EventSeries
 */
MEDEA::EventSeries::~EventSeries()
{
    for (auto event : events_) {
        if (event) {
            event->deleteLater();
        }
    }
}

/**
 * @brief MEDEA::EventSeries::clear
 */
void MEDEA::EventSeries::clear()
{
    auto i = events_.begin();
    while (i != events_.end()) {
        auto event = (*i);
        event->deleteLater();
        i = events_.erase(i);
    }

    // clear ids list
    event_ids_.clear();

    // reset the time range
    minTime_ = QDateTime::currentMSecsSinceEpoch();
    maxTime_ = 0;
}

/**
 * @brief MEDEA::EventSeries::isEmpty
 * @return
 */
bool MEDEA::EventSeries::isEmpty() const
{
    return events_.isEmpty();
}

/**
 * @brief MEDEA::EventSeries::getEvents
 * @return
 */
const QList<MEDEA::Event*>& MEDEA::EventSeries::getEvents() const
{
    return events_;
}

/**
 * @brief MEDEA::EventSeries::updateTimeRange
 * @param new_timestamp
 */
void MEDEA::EventSeries::updateTimeRange(qint64 new_timestamp)
{
    if (new_timestamp < minTime_) {
        minTime_ = new_timestamp;
        emit minTimeChanged(minTime_);
    }
    if (new_timestamp > maxTime_) {
        maxTime_ = new_timestamp;
        emit maxTimeChanged(maxTime_);
    }
}

/**
 * @brief MEDEA::EventSeries::setLabel
 * @param label
 */
void MEDEA::EventSeries::setLabel(const QString& label)
{
    label_ = label;
}

/**
 * @brief MEDEA::EventSeries::getLabel
 * @return
 */
const QString& MEDEA::EventSeries::getLabel() const
{
    return label_;
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
MEDEA::ChartDataKind MEDEA::EventSeries::getKind() const
{
    return kind_;
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
 * @brief MEDEA::EventSeries::getEventSeriesID
 * @return
 */
const QString& MEDEA::EventSeries::getEventSeriesID() const
{
    return eventSeriesIDStr_;
}

/**
 * @brief MEDEA::EventSeries::getPreviousTime
 * @param time
 * @return
 */
qint64 MEDEA::EventSeries::getPreviousTime(qint64 time) const
{
    if (events_.isEmpty()) {
        return time;
    }

    // This returns an iterator to the first event whose time is greater than or equal to the provided time
    auto event_itr = getFirstAfterTime(time);

    // If the returned iterator points to the first event, return it only if it's before the provided time
    if (event_itr == events_.begin()) {
        return qMin((*event_itr)->getTimeMS(), time);
    } else {
        event_itr--;
        return (*event_itr)->getTimeMS();
    }
}

/**
 * @brief MEDEA::EventSeries::getNextTime
 * @param time
 * @return
 */
qint64 MEDEA::EventSeries::getNextTime(qint64 time) const
{
    // This returns an iterator to the first event whose time is greater than or equal to the provided time
    auto event_itr = getFirstAfterTime(time);

    // Get the iterator to the first event whose time is greater than the provided time
    while (event_itr != events_.end()) {
        auto&& event_time = (*event_itr)->getTimeMS();
        if (event_time > time) {
            return event_time;
        }
        event_itr++;
    }

    return time;
}

/**
 * @brief MEDEA::EventSeries::getFirstAfterTime
 * This returns a const iterator that points to the first item whose time is greater than or equal to the provided time
 * @param timeMS
 * @return
 */
QList<MEDEA::Event*>::const_iterator MEDEA::EventSeries::getFirstAfterTime(const qint64 timeMS) const
{
    auto firstItr = std::lower_bound(events_.cbegin(), events_.cend(), timeMS, [](const Event* e, const qint64 &time) {
        return e->getTimeMS() < time;
    });
    return firstItr;
}

/**
 * @brief MEDEA::EventSeries::getEventsBetween
 * @param fromTimeMS - If -1 is provided, use the min time of the series
 * @param toTimeMS - If -1 is provided, use the max time of the series
 * @return
 */
QList<MEDEA::Event*> MEDEA::EventSeries::getEventsBetween(qint64 fromTimeMS, qint64 toTimeMS) const
{
    if (fromTimeMS == -1 && toTimeMS == -1) {
        return events_;
    }

    QList<Event*> events;
    if (fromTimeMS == -1) {
        if (toTimeMS < minTime_) {
            return events;
        }
        fromTimeMS = minTime_;
    } else if (toTimeMS == -1) {
        if (fromTimeMS > maxTime_) {
            return events;
        }
        toTimeMS = maxTime_;
    }

    // Get the iterator to the first event within the given range
    auto fromItr = getFirstAfterTime(fromTimeMS);
    auto current = fromItr;

    // Get the iterator to the last event within the given range
    auto toItr = getFirstAfterTime(toTimeMS);
    while (toItr != events_.end()) {
        auto toTime = (*toItr)->getTimeMS();
        if (toTime > toTimeMS) {
            break;
        }
        toItr++;
    }

    // Return the events up to, but not including the event the toItr points to
    while (current != toItr) {
        auto event = (*current);
        events.append(event);
        current++;
    }

    return events;
}

/**
 * @brief MEDEA::EventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString MEDEA::EventSeries::getHoveredDataString (
        qint64 fromTimeMS,
        qint64 toTimeMS,
        int numberOfItemsToDisplay,
        const QString& displayFormat) const
{
    if (isEmpty()) {
        return "";
    }
    // get the iterators to the first and last events within the hovered range
    auto fromItr = getFirstAfterTime(fromTimeMS);
    auto toItr = getFirstAfterTime(toTimeMS);
    return getDataString(fromItr, toItr, numberOfItemsToDisplay, displayFormat);
}

/**
 * @brief MEDEA::EventSeries::getDataString
 * @param from
 * @param to
 * @param numberOfItems
 * @param dateTimeFormat
 * @return
 */
QString MEDEA::EventSeries::getDataString (
        const QList<Event*>::const_iterator& from,
        const QList<Event*>::const_iterator& to,
        int numberOfItems,
        const QString& dateTimeFormat) const
{
    QString hoveredData;
    QTextStream stream(&hoveredData);
    int displayCount = 0;
    auto current = from;

    while (displayCount < numberOfItems) {
        // return if there are no more events to check
        if (current == to) {
            return hoveredData.trimmed();
        }
        auto event = (*current);
        stream << event->toString(dateTimeFormat);
        current++;
        displayCount++;
    }

    // we didn't go through all the items; notify the user that items were omitted
    if (current != to) {
        stream << "... (more omitted)";
    }

    return hoveredData.trimmed();
}

/**
 * @brief MEDEA::EventSeries::contains
 * @param event
 * @return
 */
bool MEDEA::EventSeries::contains(MEDEA::Event* event) const
{
    return event_ids_.contains(event->getID());
}

/**
 * @brief MEDEA::EventSeries::addEventToList
 * @param event
 * @throw std::invalid_argument
 */
void MEDEA::EventSeries::addEventToList(MEDEA::Event& event)
{

    auto&& new_event = &event;
    if (contains(new_event)) {
        throw std::invalid_argument("MEDEA::EventSeries::addEventToList - Cannot add an already existing event");
    }

    // Add the new event to the list, update the time range
    // and send the signal to notify that a new event has been added
    events_.append(new_event);
    event_ids_.append(event.getID());
    updateTimeRange(new_event->getTimeMS());
    emit eventAdded(new_event);
}

/**
 * @brief MEDEA::EventSeries::getDefaultDisplayFormat
 * @return
 */
const QString& MEDEA::EventSeries::getDefaultDisplayFormat()
{
    static QString displayFormat = "MMM d, hh:mm:ss.zzz";
    return displayFormat;
}

/**
 * @brief MEDEA::EventSeries::getDefaultNumberOfItemsToDisplay
 * @return
 */
int MEDEA::EventSeries::getDefaultNumberOfItemsToDisplay()
{
    static int numberToDisplay = 10;
    return numberToDisplay;
}