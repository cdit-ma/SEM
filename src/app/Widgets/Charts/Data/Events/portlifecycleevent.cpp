#include "portlifecycleevent.h"

/**
 * @brief PortLifecycleEvent::PortLifecycleEvent
 * @param parent
 */
PortLifecycleEvent::PortLifecycleEvent(Port port, LifecycleType type, qint64 time, QObject* parent)
    : MEDEA::Event(time, port.name, parent)
{
    port_ = port;
    type_ = type;
}


/**
 * @brief PortLifecycleEvent::getPort
 * @return
 */
const Port PortLifecycleEvent::getPort()
{
    return port_;
}


/**
 * @brief PortLifecycleEvent::getType
 * @return
 */
const LifecycleType PortLifecycleEvent::getType()
{
    return type_;
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
