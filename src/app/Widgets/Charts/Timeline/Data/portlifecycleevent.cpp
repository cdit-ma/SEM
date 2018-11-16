#include "portlifecycleevent.h"

/**
 * @brief PortLifecycleEvent::PortLifecycleEvent
 * @param parent
 */
PortLifecycleEvent::PortLifecycleEvent(Port port, LifecycleType type, qint64 time, QObject* parent)
    : QObject(parent)
{
    port_ = port;
    type_ = type;
    time_ = time;
}


/**
 * @brief PortLifecycleEvent::getPort
 * @return
 */
Port PortLifecycleEvent::getPort()
{
    return port_;
}


/**
 * @brief PortLifecycleEvent::getType
 * @return
 */
LifecycleType PortLifecycleEvent::getType()
{
    return type_;
}


/**
 * @brief PortLifecycleEvent::getTime
 * @return
 */
qint64 PortLifecycleEvent::getTime()
{
    return time_;
}


/**
 * @brief PortLifecycleEvent::getPortGraphmlID
 * @return
 */
const QString PortLifecycleEvent::getPortGraphmlID()
{
    return port_.graphml_id;
}


/**
 * @brief PortLifecycleEvent::getPortPath
 * @return
 */
const QString PortLifecycleEvent::getPortPath()
{
    return port_.path;
}
