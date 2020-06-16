#include "utilisationrequest.h"

/**
 * @brief UtilisationRequest::setNodeIDs
 * @param node_ids
 */
void UtilisationRequest::setNodeIDs(const QVector<QString>& node_ids)
{
    nodeIDs_ = node_ids;
}

/**
 * @brief UtilisationRequest::setNodeHostnames
 * @param node_hostnames
 */
void UtilisationRequest::setNodeHostnames(const QVector<QString>& node_hostnames)
{
    nodeHostnames_ = node_hostnames;
}

/**
 * @brief UtilisationRequest::node_ids
 * @return
 */
const QVector<QString>& UtilisationRequest::node_ids() const
{
    return nodeIDs_;
}

/**
 * @brief UtilisationRequest::node_hostnames
 * @return
 */
const QVector<QString>& UtilisationRequest::node_hostnames() const
{
    return nodeHostnames_;
}