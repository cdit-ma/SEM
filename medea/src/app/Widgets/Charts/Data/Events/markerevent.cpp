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
    : MEDEA::Event(MEDEA::ChartDataKind::MARKER, time, parent),
      marker_name_(markerName),
      id_(inst.graphml_id + markerName + QString::number(time)),
      markerID_(markerID) {}

/**
 * @brief MarkerEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString MarkerEvent::toString(const QString& dateTimeFormat) const
{
	return "Marker Name: " + marker_name_ + "\n" +
		   "ID: " + QString::number(markerID_) + "\n" +
		   "At: " + getDateTimeString(dateTimeFormat) + "\n\n";
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