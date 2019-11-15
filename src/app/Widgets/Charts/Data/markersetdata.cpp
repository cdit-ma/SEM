#include "markersetdata.h"
#include "../ExperimentDataManager/experimentdatamanager.h"

std::atomic<int> MarkerSetData::marker_set_id(0);

/**
 * @brief MarkerSetData::MarkerSetData
 * @param exp_run_id
 * @param marker_name
 * @param parent
 */
MarkerSetData::MarkerSetData(quint32 exp_run_id, const QString& marker_name, QObject* parent)
    : QObject(parent),
      experiment_run_id_(exp_run_id),
      last_updated_time_(0),
      marker_name_(marker_name),
      marker_set_id_(marker_set_id++)
{
    marker_event_series_ = new MarkerEventSeries(marker_name);
    marker_event_series_->setLabel(marker_name);

    connect(this, &MarkerSetData::requestData, ExperimentDataManager::manager(), &ExperimentDataManager::requestMarkerSetEvents);
}


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
 * @brief MarkerSetData::getMarkerRequest
 * @return
 */
const MarkerRequest& MarkerSetData::getMarkerRequest() const
{
    return marker_request_;
}


/**
 * @brief MarkerSetData::addMarkerEvents
 * @param events
 */
void MarkerSetData::addMarkerEvents(const QVector<MarkerEvent*>& events)
{
    qDebug() << "Received Marker Events#: " << events.size();
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


/**
 * @brief MarkerSetData::updateData
 * @param last_updated_time
 */
void MarkerSetData::updateData(qint64 last_updated_time)
{
    // NOTE: The requests need to be setup/updated before this signal is sent
    if (last_updated_time > last_updated_time_) {
        marker_request_.setTimeInterval({last_updated_time});
        marker_request_.setTimeInterval({last_updated_time});
        last_updated_time_ = last_updated_time;
        emit requestData(*this);
    }
}
