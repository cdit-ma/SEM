#include "portinstancedata.h"

/**
 * @brief PortInstanceData::PortInstanceData
 * @param port
 */
PortInstanceData::PortInstanceData(const AggServerResponse::Port& port)
    : graphml_id_(port.graphml_id),
      name_(port.name),
      path_(port.path),
      middleware_(port.middleware),
      kind_(port.kind)
{
    port_lifecycle_series_ = new PortLifecycleEventSeries(graphml_id_);
    port_lifecycle_series_->setProperty("name", name_);
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
 * @brief PortInstanceData::addPortLifecycleEvents
 * @param events
 */
void PortInstanceData::addPortLifecycleEvents(const QVector<PortLifecycleEvent*>& events)
{
    port_lifecycle_series_->addEvents(events);
}


/**
 * @brief PortInstanceData::getPortLifecycleEventSeries
 * @return
 */
PortLifecycleEventSeries* PortInstanceData::getPortLifecycleEventSeries() const
{
    return port_lifecycle_series_;
}
