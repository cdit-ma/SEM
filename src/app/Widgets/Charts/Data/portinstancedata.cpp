#include "portinstancedata.h"
#include "componentinstancedata.h"
#include "../ExperimentDataManager/experimentdatamanager.h"

/**
 * @brief PortInstanceData::PortInstanceData
 * @param exp_run_id
 * @param comp_inst
 * @param port
 * @param parent
 */
PortInstanceData::PortInstanceData(quint32 exp_run_id, const ComponentInstanceData& comp_inst, const AggServerResponse::Port& port, QObject* parent)
    : QObject(parent),
      experiment_run_id_(exp_run_id),
      last_updated_time_(0),
      graphml_id_(port.graphml_id),
      name_(port.name),
      path_(port.path),
      middleware_(port.middleware),
      kind_(port.kind)
{
    // Setup the requests
    port_lifecycle_request_.setExperimentRunID(exp_run_id);
    port_lifecycle_request_.setPortIDs({graphml_id_});
    port_lifecycle_request_.setPortPaths({path_});
    //port_lifecycle_request_.setComponentInstanceIDS({comp_inst.getGraphmlID()});
    //port_lifecycle_request_.setComponentInstancePaths({comp_inst.getPath()});

    port_event_request_.setExperimentRunID(exp_run_id);
    port_event_request_.setPortIDs({graphml_id_});
    port_event_request_.setPortPaths({path_});

    // TODO: This extra request fields are returning unexpected events
    // When they are added, they also return port events from other children ports of the parenting ComponentInstance
    //port_event_request_.setComponentInstanceIDS({comp_inst.getGraphmlID()});
    //port_event_request_.setComponentInstancePaths({comp_inst.getPath()});

    // Setup event series
    port_lifecycle_series_ = new PortLifecycleEventSeries(graphml_id_);
    port_lifecycle_series_->setLabel(name_);

    port_event_series_ = new PortEventSeries(graphml_id_);
    port_event_series_->setLabel(name_);

    connect(this, &PortInstanceData::requestData, ExperimentDataManager::manager(), &ExperimentDataManager::requestPortInstanceEvents);
    emit requestData(*this);
}


/**
 * @brief PortInstanceData::getGraphmlID
 * @return
 */
const QString& PortInstanceData::getGraphmlID() const
{
    return graphml_id_;
}


/**
 * @brief PortInstanceData::getName
 * @return
 */
const QString& PortInstanceData::getName() const
{
    return name_;
}


/**
 * @brief PortInstanceData::getPath
 * @return
 */
const QString& PortInstanceData::getPath() const
{
    return path_;
}


/**
 * @brief PortInstanceData::getMiddleware
 * @return
 */
const QString& PortInstanceData::getMiddleware() const
{
    return middleware_;
}


/**
 * @brief PortInstanceData::getKind
 * @return
 */
AggServerResponse::Port::Kind PortInstanceData::getKind() const
{
    return kind_;
}


/**
 * @brief PortInstanceData::getPortLifecycleRequest
 * @return
 */
const PortLifecycleRequest& PortInstanceData::getPortLifecycleRequest() const
{
    return port_lifecycle_request_;
}


/**
 * @brief PortInstanceData::getPortEventRequest
 * @return
 */
const PortEventRequest& PortInstanceData::getPortEventRequest() const
{
    return port_event_request_;
}


/**
 * @brief PortInstanceData::getPreviousEventTime
 * @param time
 * @return
 */
qint64 PortInstanceData::getPreviousEventTime(qint64 time) const
{
    const auto& prev_port_lifecycle_event_time = getPortLifecycleEventSeries().getPreviousTime(time);
    const auto& prev_port_event_time = getPortEventSeries().getPreviousTime(time);

    if (prev_port_lifecycle_event_time == time) {
        return prev_port_event_time;
    }
    if (prev_port_event_time == time) {
        return prev_port_lifecycle_event_time;
    }

    return qMax(prev_port_lifecycle_event_time, prev_port_event_time);
}


/**
 * @brief PortInstanceData::getNextEventTime
 * @param time
 * @return
 */
qint64 PortInstanceData::getNextEventTime(qint64 time) const
{
    const auto& next_port_lifecycle_event_time = getPortLifecycleEventSeries().getNextTime(time);
    const auto& next_port_event_time = getPortEventSeries().getNextTime(time);

    if (next_port_lifecycle_event_time == time) {
        return next_port_event_time;
    }
    if (next_port_event_time == time) {
        return next_port_lifecycle_event_time;
    }

    return qMin(next_port_lifecycle_event_time, next_port_event_time);
}


/**
 * @brief PortInstanceData::addPortLifecycleEvents
 * @param events
 */
void PortInstanceData::addPortLifecycleEvents(const QVector<PortLifecycleEvent*>& events)
{
    //qDebug() << name_ << " - Received Port Lifecycle Events#: " << events.size();
    port_lifecycle_series_->addEvents(events);
}


/**
 * @brief PortInstanceData::getPortLifecycleEventSeries
 * @return
 */
const PortLifecycleEventSeries& PortInstanceData::getPortLifecycleEventSeries() const
{
    if (port_lifecycle_series_ == nullptr) {
        throw std::runtime_error("PortEventSeries& PortInstanceData::getPortLifecycleEventSeries - Port lifecycle event series is null");
    }
    return *port_lifecycle_series_;
}


/**
 * @brief PortInstanceData::addPortEvents
 * @param events
 */
void PortInstanceData::addPortEvents(const QVector<PortEvent*>& events)
{
    //qDebug() << name_ << " - Received Port Events#: " << events.size();
    port_event_series_->addEvents(events);
}


/**
 * @brief PortInstanceData::getPortEventSeries
 * @throws std::runtime_error
 * @return
 */
const PortEventSeries& PortInstanceData::getPortEventSeries() const
{
    if (port_event_series_ == nullptr) {
        throw std::runtime_error("PortEventSeries& PortInstanceData::getPortEventSeries - Port event series is null");
    }
    return *port_event_series_;
}


/**
 * @brief PortInstanceData::updateData
 * This is called when the ExperimentRunData's last updated time has changed
 * It sets the new time interval for the particular event requests that will
 * be used by the ExperimentDataManager to update the corresponding event series
 * @param new_last_updated_time
 */
void PortInstanceData::updateData(qint64 new_last_updated_time)
{
    // Setup/update the requests before sending the signal
    port_lifecycle_request_.setTimeInterval({last_updated_time_, new_last_updated_time});
    port_event_request_.setTimeInterval({last_updated_time_, new_last_updated_time});
    last_updated_time_ = new_last_updated_time;
    emit requestData(*this);
}
