#include "porteventrequest.h"


/**
 * @brief PortEventRequest::PortEventRequest
 */
PortEventRequest::PortEventRequest() : EventRequest(MEDEA::ChartDataKind::PORT_EVENT) {}


/**
 * @brief PortEventRequest::setPortIDs
 * @param port_ids
 */
void PortEventRequest::setPortIDs(const QVector<QString>& port_ids)
{
    portIDs_ = port_ids;
}


/**
 * @brief PortEventRequest::setPortPaths
 * @param port_paths
 */
void PortEventRequest::setPortPaths(const QVector<QString>& port_paths)
{
    portPaths_ = port_paths;
}


/**
 * @brief PortEventRequest::port_ids
 * @return
 */
const QVector<QString>& PortEventRequest::port_ids() const
{
    return portIDs_;
}


/**
 * @brief PortEventRequest::port_paths
 * @return
 */
const QVector<QString>& PortEventRequest::port_paths() const
{
    return portPaths_;
}
