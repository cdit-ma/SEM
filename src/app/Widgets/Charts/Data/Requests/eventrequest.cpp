#include "eventrequest.h"


/**
 * @brief EventRequest::EventRequest
 * @param kind
 */
EventRequest::EventRequest(TIMELINE_DATA_KIND kind) : Request(kind) {}


/**
 * @brief EventRequest::setComponentNames
 * @param component_names
 */
void EventRequest::setComponentNames(const QVector<QString> &component_names)
{
    componentNames_ = component_names;
}


/**
 * @brief EventRequest::setComponentInstanceIDS
 * @param component_instance_ids
 */
void EventRequest::setComponentInstanceIDS(const QVector<QString> &component_instance_ids)
{
    componentInstanceIDs_ = component_instance_ids;
}


/**
 * @brief EventRequest::setComponentInstancePaths
 * @param component_instance_paths
 */
void EventRequest::setComponentInstancePaths(const QVector<QString> &component_instance_paths)
{
    componentInstancePaths_ = component_instance_paths;
}


/**
 * @brief EventRequest::component_names
 * @return
 */
const QVector<QString> &EventRequest::component_names() const
{
    return componentNames_;
}


/**
 * @brief EventRequest::component_instance_ids
 * @return
 */
const QVector<QString> &EventRequest::component_instance_ids() const
{
    return componentInstanceIDs_;
}


/**
 * @brief EventRequest::component_instance_paths
 * @return
 */
const QVector<QString> &EventRequest::component_instance_paths() const
{
    return componentInstancePaths_;
}
