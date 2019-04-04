#include "markereventseries.h"
#include "../Events/markerevent.h"

#include <QSet>

/**
 * @brief MarkerEventSeries::MarkerEventSeries
 * @param ID
 * @param parent
 */
MarkerEventSeries::MarkerEventSeries(const QString& ID, QObject *parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::MARKER, parent) {}


/**
 * @brief MarkerEventSeries::getMarkerIDSetRanges
 * @return
 */
const QHash<qint64, QPair<qint64, qint64> > &MarkerEventSeries::getMarkerIDSetRanges() const
{
    return markerIDSetRanges_;
}

/**
 * @brief MarkerEventSeries::getMarkerIDsWithSharedStartTimes
 * @return
 */
const QMap<qint64, QSet<qint64> > &MarkerEventSeries::getMarkerIDsWithSharedStartTimes() const
{
    return startTimeMap_;
}


/**
 * @brief MarkerEventSeries::getMarkerIDSetDurations
 * @return
 */
const QHash<qint64, qint64> &MarkerEventSeries::getMarkerIDSetDurations() const
{
    return markerIDSetDurations_;
}


/**
 * @brief MarkerEventSeries::addEvent
 * @param event
 */
void MarkerEventSeries::addEvent(MEDEA::Event* event)
{
    if (event->getKind() != MEDEA::ChartDataKind::MARKER)
        return;

    auto markerEvent = (MarkerEvent*)event;
    auto markerID = markerEvent->getMarkerID();
    auto minTime = event->getTimeMS();
    auto maxTime = event->getTimeMS();

    if (markerIDSetRanges_.contains(markerID)) {
        minTime = qMin(minTime, markerIDSetRanges_.value(markerID).first);
        maxTime = qMax(maxTime, markerIDSetRanges_.value(markerID).second);
    }

    markerIDSetRanges_.insert(markerID, {minTime, maxTime});
    markerIDSetDurations_.insert(markerID, maxTime - minTime);

    if (!startTimeMap_.contains(minTime)) {
        startTimeMap_.insert(minTime, {markerID});
    } else {
        startTimeMap_[minTime].insert(markerID);
    }

    MEDEA::EventSeries::addEvent(event);
}


/**
 * @brief MarkerEventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString MarkerEventSeries::getHoveredDataString (
        qint64 fromTimeMS,
        qint64 toTimeMS,
        int numberOfItemsToDisplay,
        const QString& displayFormat) const
{
    Q_UNUSED(numberOfItemsToDisplay)
    Q_UNUSED(displayFormat)

    if (isEmpty() || startTimeMap_.isEmpty()) {
        return "";
    }

    const auto& startTimes = startTimeMap_.keys();
    auto current = std::lower_bound(startTimes.constBegin(), startTimes.constEnd(), fromTimeMS, [](const qint64 &idSetStartTime, const qint64 &time) {
        return idSetStartTime < time;
    });
    auto upper = std::upper_bound(startTimes.constBegin(), startTimes.constEnd(), toTimeMS, [](const qint64 &time, const qint64 &idSetStartTime) {
        return time < idSetStartTime;
    });
    return getDataString(current, upper);
}


/**
 * @brief MarkerEventSeries::getDataString
 * @param from
 * @param to
 * @return
 */
QString MarkerEventSeries::getDataString (
        const QList<qint64>::const_iterator& from,
        const QList<qint64>::const_iterator& to) const
{
    // calculate the average duration between from time and to time
    qint64 totalDurationMS = 0;
    int numberOfIDSets = 0;
    auto current = from;

    while (current != to) {
        auto currentStartTime = (*current);
        const auto& markerIDsAtStartTime = startTimeMap_.value(currentStartTime);
        for (const auto& id : markerIDsAtStartTime) {
            if (markerIDSetDurations_.contains(id)) {
                totalDurationMS += markerIDSetDurations_.value(id);
            }
            numberOfIDSets++;
        }
        current++;
    }

    QString hoveredData;
    QTextStream stream(&hoveredData);

    if (numberOfIDSets > 0) {
        double avgDuration = (double) totalDurationMS / numberOfIDSets;
        stream << "Marker ID Sets Started#: " << numberOfIDSets << "\n"
               << "Average Duration: " << avgDuration << "ms \n\n";
    }

    return hoveredData.trimmed();
}
