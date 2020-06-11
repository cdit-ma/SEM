#include "markersetdata.h"
#include "../ExperimentDataManager/experimentdatamanager.h"

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
      marker_name_(marker_name)
{
    setupSeries();

    connect(this, &MarkerSetData::requestData, ExperimentDataManager::manager(), &ExperimentDataManager::requestMarkerSetEvents);
    emit requestData(*this);
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
    marker_event_series_->addEvents(events);
}

/**
 * @brief MarkerSetData::getMarkerEventSeries
 * @throws std::runtime_error
 * @return
 */
QPointer<const MEDEA::EventSeries> MarkerSetData::getMarkerEventSeries() const
{
	if (marker_event_series_ == nullptr) {
		throw std::runtime_error("MarkerSetData::getMarkerEventSeries - Marker event series is null");
	}
	return marker_event_series_;
}

/**
 * @brief MarkerSetData::updateData
 * This is called when the ExperimentRunData's last updated time has changed
 * It sets the new time interval for the particular event requests that will
 * be used by the ExperimentDataManager to update the corresponding event series
 * @param new_last_updated_time
 */
void MarkerSetData::updateData(qint64 new_last_updated_time)
{
    // Setup/update the requests before sending the signal
    marker_request_.setTimeInterval({last_updated_time_, new_last_updated_time});
    last_updated_time_ = new_last_updated_time;
    emit requestData(*this);
}

/**
 * @brief NodeData::setupSeries
 */
void MarkerSetData::setupSeries()
{
    auto&& exp_run_id_str = QString::number(experiment_run_id_);

    marker_event_series_ = new MarkerEventSeries(marker_name_ + exp_run_id_str);
    marker_event_series_->setLabel("[" + exp_run_id_str + "] " + marker_name_);
    marker_event_series_->setParent(this);
}