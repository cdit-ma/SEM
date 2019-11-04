#include "markersetdata.h"

std::atomic<int> MarkerSetData::marker_set_id(0);

/**
 * @brief MarkerSetData::MarkerSetData
 * @param marker_name
 */
MarkerSetData::MarkerSetData(const QString& marker_name)
    : marker_name_(marker_name),
      marker_set_id_(marker_set_id++) {}


/**
 * @brief MarkerSetData::getID
 * @return
 */
int MarkerSetData::getID() const
{
    return marker_set_id_;
}


/**
 * @brief MarkerSetData::getMarkerName
 * @return
 */
const QString& MarkerSetData::getMarkerName() const
{
    return marker_name_;
}


/**
 * @brief MarkerSetData::addMarkerEvents
 * @param events
 */
void MarkerSetData::addMarkerEvents(const QVector<MarkerEvent*>& events)
{
    if (events.isEmpty()) {
        return;
    }

    if (marker_event_series_ == nullptr) {
        marker_event_series_ = new MarkerEventSeries(getMarkerName());
        marker_event_series_->setProperty("label", getMarkerName());
    }

    marker_event_series_->addEvents(events);
}


/**
 * @brief MarkerSetData::getMarkerEventSeries
 * @return
 */
MarkerEventSeries* MarkerSetData::getMarkerEventSeries() const
{
    return marker_event_series_;
}
