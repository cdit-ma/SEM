#include "markerevent.h"

/**
 * @brief MarkerEvent::MarkerEvent
 * @param markerName
 * @param markerID
 * @param inst
 * @param time
 * @param parent
 */
MarkerEvent::MarkerEvent(const QString& markerName,
                         qint64 markerID,
                         const AggServerResponse::ComponentInstance& inst,
                         qint64 time,
                         QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::MARKER, time, markerName, parent),
      series_id_(inst.graphml_id + markerName),
      id_(series_id_ + QString::number(time)),
      markerID_(markerID) {}


/**
 * @brief MarkerEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString MarkerEvent::toString(const QString &dateTimeFormat) const
{
    return "Marker Name: " + getSeriesName() + "\n" +
            "ID: " + QString::number(markerID_) + "\n" +
            "At: " + getDateTimeString(dateTimeFormat) + "\n\n";
}


/**
 * @brief MarkerEvent::getSeriesID
 * @return
 */
const QString& MarkerEvent::getSeriesID() const
{
    return series_id_;
}


/**
 * @brief MarkerEvent::getID
 * @return
 */
const QString& MarkerEvent::getID() const
{
    return id_;
}


/**
 * @brief MarkerEvent::getMarkerID
 * @return
 */
qint64 MarkerEvent::getMarkerID() const
{
    return markerID_;
}


