#include "markereventseries.h"
#include "../Events/markerevent.h"

#include <QDebug>

/**
 * @brief MarkerEventSeries::MarkerEventSeries
 * @param ID
 * @param parent
 */
MarkerEventSeries::MarkerEventSeries(QString ID, QObject *parent)
    : MEDEA::EventSeries(ID, TIMELINE_DATA_KIND::MARKER, parent) {}


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
    if (event->getKind() != TIMELINE_DATA_KIND::MARKER)
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
QString MarkerEventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat)
{
    //qDebug() << "FROM: " << QDateTime::fromMSecsSinceEpoch(fromTimeMS).toString(displayFormat);
    //qDebug() << "TO: " << QDateTime::fromMSecsSinceEpoch(toTimeMS).toString(displayFormat);

    const auto& startTimes = startTimeMap_.keys();
    /*auto current = std::lower_bound(startTimes.constBegin(), startTimes.constEnd(), fromTimeMS, [](const qint64 &idSetStartTime, const qint64 &time) {
        return idSetStartTime < time;
    });
    auto upper = std::upper_bound(startTimes.constBegin(), startTimes.constEnd(), toTimeMS, [](const qint64 &time, const qint64 &idSetStartTime) {
        return time < idSetStartTime;
    });*/

    auto currentStartTimeItr = startTimes.constBegin();
    auto endStartTimeItr = startTimes.constEnd();
    auto firstItrSet = false;

    for (auto current = startTimes.constBegin(); current != startTimes.constEnd(); current++) {
        auto currentStartTime = (*current);
        if (!firstItrSet) {
            if (currentStartTime > toTimeMS)
                return "";
            if (currentStartTime > fromTimeMS) {
                currentStartTimeItr = current;
                firstItrSet = true;
            }
        }
        if (currentStartTime > toTimeMS) {
            if (current == startTimes.constBegin()) {
                endStartTimeItr = startTimes.constBegin();
            } else {
                endStartTimeItr = current - 1;
            }
            break;
        }
    }

    if (!firstItrSet)
        return "";

    QString hoveredData;
    QTextStream stream(&hoveredData);
    int count = std::distance(currentStartTimeItr, endStartTimeItr);
    numberOfItemsToDisplay = qMin(count, numberOfItemsToDisplay);
    numberOfItemsToDisplay = qMax(1, numberOfItemsToDisplay);

    // calculate the average duration between fromTimeMS and toTimeMS
    auto totalDuration = 0.0;
    auto numberOfIDSets = 0;
    for (int i = 0; i < numberOfItemsToDisplay; i++) {
        auto currentStartTime = (*currentStartTimeItr);
        const auto& markerIDsAtStartTime = startTimeMap_.value(currentStartTime);
        for (const auto& id : markerIDsAtStartTime) {
            if (markerIDSetDurations_.contains(id)) {
                totalDuration += markerIDSetDurations_.value(id);
            }
            numberOfIDSets++;
        }
        currentStartTimeItr++;
    }

    if (numberOfIDSets == 0)
        return "";

    auto avgDuration = totalDuration / numberOfIDSets;
    stream << "Marker ID Sets Started#: " << numberOfIDSets << "\n"
           << "Average Duration: " << avgDuration << "ms \n\n";

    return hoveredData.trimmed();
}
