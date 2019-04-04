#include "portlifecyclerequest.h"


/**
 * @brief PortLifecycleRequest::PortLifecycleRequest
 */
PortLifecycleRequest::PortLifecycleRequest() : EventRequest(MEDEA::ChartDataKind::PORT_LIFECYCLE) {}


/**
 * @brief PortLifecycleRequest::setPortIDs
 * @param port_ids
 */
void PortLifecycleRequest::setPortIDs(const QVector<QString> &port_ids)
{
    portIDs_ = port_ids;
}


/**
 * @brief PortLifecycleRequest::setPortPaths
 * @param port_paths
 */
void PortLifecycleRequest::setPortPaths(const QVector<QString> &port_paths)
{
    portPaths_ = port_paths;
}


/**
 * @brief PortLifecycleRequest::port_ids
 * @return
 */
const QVector<QString> &PortLifecycleRequest::port_ids() const
{
    return portIDs_;
}


/**
 * @brief PortLifecycleRequest::port_paths
 * @return
 */
const QVector<QString> &PortLifecycleRequest::port_paths() const
{
    return portPaths_;
}
