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
    port_lifecycle_request_.setComponentInstanceIDS({comp_inst.getGraphmlID()});
    port_lifecycle_request_.setComponentInstancePaths({comp_inst.getPath()});

    port_event_request_.setExperimentRunID(exp_run_id);
    port_event_request_.setPortIDs({graphml_id_});
    port_event_request_.setPortPaths({path_});
    port_event_request_.setComponentInstanceIDS({comp_inst.getGraphmlID()});
    port_event_request_.setComponentInstancePaths({comp_inst.getPath()});

    // Setup event series
    port_lifecycle_series_ = new PortLifecycleEventSeries(graphml_id_);
    port_lifecycle_series_->setLabel(name_);

    port_event_series_ = new PortEventSeries(graphml_id_);
    port_event_series_->setLabel(name_);

    connect(this, &PortInstanceData::requestData, ExperimentDataManager::manager(), &ExperimentDataManager::requestPortInstanceEvents);
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
 * @brief PortInstanceData::addPortLifecycleEvents
 * @param events
 */
void PortInstanceData::addPortLifecycleEvents(const QVector<PortLifecycleEvent*>& events)
{
    qDebug() << "\nReceived Port Lifecycle Events#: " << events.size();
    port_lifecycle_series_->addEvents(events);
    qDebug() << "----------------------- END addPortLifecycleEvents";
}


/**
 * @brief PortInstanceData::addPortEvents
 * @param events
 */
void PortInstanceData::addPortEvents(const QVector<PortEvent*>& events)
{
    qDebug() << "\nReceived Port Events#: " << events.size();
    port_event_series_->addEvents(events);
}


/**
 * @brief PortInstanceData::getPortLifecycleEventSeries
 * @return
 */
PortLifecycleEventSeries* PortInstanceData::getPortLifecycleEventSeries() const
{
    return port_lifecycle_series_;
}


/**
 * @brief PortInstanceData::updateData
 * @param last_updated_time
 */
void PortInstanceData::updateData(qint64 last_updated_time)
{
    // NOTE: The requests need to be setup/updated before this signal is sent
    if (last_updated_time > last_updated_time_) {
        port_lifecycle_request_.setTimeInterval({last_updated_time});
        port_event_request_.setTimeInterval({last_updated_time});
        last_updated_time_ = last_updated_time;
        emit requestData(*this);
    }
}
