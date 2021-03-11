#include "hardwaremetricrequest.h"

/**
 * @brief HardwareMetricRequest::setNodeIDs
 * @param node_ids
 */
void HardwareMetricRequest::setNodeIDs(const QVector<QString>& node_ids)
{
    nodeIDs_ = node_ids;
}

/**
 * @brief HardwareMetricRequest::setNodeHostnames
 * @param node_hostnames
 */
void HardwareMetricRequest::setNodeHostnames(const QVector<QString>& node_hostnames)
{
    nodeHostnames_ = node_hostnames;
}

/**
 * @brief HardwareMetricRequest::node_ids
 * @return
 */
const QVector<QString>& HardwareMetricRequest::node_ids() const
{
    return nodeIDs_;
}

/**
 * @brief HardwareMetricRequest::node_hostnames
 * @return
 */
const QVector<QString>& HardwareMetricRequest::node_hostnames() const
{
    return nodeHostnames_;
}