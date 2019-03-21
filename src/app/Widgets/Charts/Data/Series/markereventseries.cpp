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

    //qDebug() << "markerID: " << markerID;
    //qDebug() << "eventime: " << QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("MMM d, hh:mm:ss.zzz");

    /*qDebug() << "markerID: " << markerID;
    qDebug() << "eventime: " << QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("MMM d, hh:mm:ss.zzz");
    qDebug() << "minTime: " << QDateTime::fromMSecsSinceEpoch(minTime).toString("MMM d, hh:mm:ss.zzz");
    qDebug() << "maxTime: " << QDateTime::fromMSecsSinceEpoch(maxTime).toString("MMM d, hh:mm:ss.zzz");
    qDebug() << "duration: " << markerIDSetDurations_.value(markerID);
    qDebug() << "--------------------";*/

    if (!startTimeMap_.contains(minTime)) {
        startTimeMap_.insert(minTime, {markerID});
        //qDebug() << "INSERT START TIME: " << QDateTime::fromMSecsSinceEpoch(minTime).toString("MMM d, hh:mm:ss.zzz");
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

    auto lower = startTimes.constBegin();
    auto upper = startTimes.constEnd();
    auto firstItrSet = false;

    for (auto current = startTimes.constBegin(); current != startTimes.constEnd(); current++) {
        auto currentStartTime = (*current);
        if (!firstItrSet) {
            if (currentStartTime > toTimeMS)
                return "";
            if (currentStartTime > fromTimeMS) {
                lower = current;
                firstItrSet = true;
                //qDebug() << "LOWER TIME: " << QDateTime::fromMSecsSinceEpoch(*lower).toString("hh:mm:ss:zzz");
            }
        }
        if (currentStartTime > toTimeMS) {
            if (current == startTimes.constBegin()) {
                upper = startTimes.constBegin();
            } else {
                upper = current - 1;
            }
            break;
        }
    }

    if (!firstItrSet)
        return "";

    QString hoveredData;
    QTextStream stream(&hoveredData);
    int count = std::distance(lower, upper);
    numberOfItemsToDisplay = qMin(count, numberOfItemsToDisplay);
    numberOfItemsToDisplay = qMax(1, numberOfItemsToDisplay);

    // calculate the average duration between fromTimeMS and toTimeMS for each contained start times
    for (int i = 0; i < numberOfItemsToDisplay; i++) {
        auto currentStartTime = (*lower);
        const auto& markerIDsAtStartTime = startTimeMap_.value(currentStartTime);
        auto numberOfIDSetsStarted = markerIDsAtStartTime.count();
        auto totalDuration = 0.0;
        for (const auto& id : markerIDsAtStartTime) {
            if (markerIDSetDurations_.contains(id)) {
                totalDuration += markerIDSetDurations_.value(id);
            }
        }
        auto avgDuration = totalDuration / numberOfIDSetsStarted;
        stream << "Start Time: " << QDateTime::fromMSecsSinceEpoch(currentStartTime).toString("hh:mm:ss:zzz") << "\n"
               << "Marker ID Sets Started#: " << numberOfIDSetsStarted << "\n"
               << "Average Duration: " << avgDuration << "ms \n\n";

        lower++;
    }

    if (count > numberOfItemsToDisplay)
        stream << "... (more omitted)";

    return hoveredData.trimmed();
}


/**
 * @brief MarkerEventSeries::getElapsedTimeString
 * @param fromMS
 * @param toMS
 * @return
 */
QString MarkerEventSeries::getElapsedTimeString(qint64 fromMS, qint64 toMS) const
{
    auto elapsedMS = toMS - fromMS;
    if (elapsedMS <= 0) {
        return "0ms";
    } else if (elapsedMS < 1000) {
        return QString::number(elapsedMS) + "ms";
    }

    auto days = elapsedMS / 8.64e7;
    auto hours = elapsedMS / 3.6e6;
    auto minutes = elapsedMS / 6e4;
    auto seconds = elapsedMS / 1000;

    /*if (maxElapsedDays >= 1) {
        // if the max displayed elapsed time is a day or more, only show the elapsed days/hours
        d = msecs / 8.64e7;
        if (d >= 100)
            return QString::number(d) + "d";
        h = (msecs / 3.6e6) - ((int)d * 24);
    } else if (maxElapsedHours >= 1) {
        // if the max displayed elapsed time is an hour or more, only show the elapsed hours/mins
        h = msecs / 3.6e6;
        if (h >= 10)
            return QString::number(h) + "h";
        m = (msecs / 6e4) - (h * 60);
    } else if (maxElapsedMins >= 1) {
        // if the max displayed elapsed time is a minute or more, only show the elapsed mins/secs
        m = msecs / 6e4;
        s = (msecs / 1e3) - (m * 60);
    } else {
        // if it's anything smaller, show the elapsed secs/msecs
        s = msecs / 1e3;
        ms = msecs - (s * 1000);
    }*/

    QString elapsedTime = "";

    if (days > 0)
        elapsedTime = QString::number(days) + "d";
    if (hours > 0)
        elapsedTime += QString::number(hours) + "h";
    if (minutes > 0)
        elapsedTime += QString::number(minutes) + "m";
    if (seconds > 0) {
        if (elapsedMS > 0) {
            elapsedTime += QString::number(seconds) + "." + QString::number(elapsedMS) + "s";
        } else {
            elapsedTime += QString::number(seconds) + "s";
        }
    }

    return elapsedTime;
}
