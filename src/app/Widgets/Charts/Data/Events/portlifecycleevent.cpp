#include "portlifecycleevent.h"

/**
 * @brief PortLifecycleEvent::PortLifecycleEvent
 * @param parent
 */
PortLifecycleEvent::PortLifecycleEvent(AggServerResponse::Port port, AggServerResponse::LifecycleType type, qint64 time, QObject* parent)
    : MEDEA::Event(time, port.name, parent)
{
    port_ = port;
    kind_ = kind;
    type_ = type;
}


/**
 * @brief PortLifecycleEvent::getPortKind
 * @return
 */
const PortLifecycleEvent::PortKind &PortLifecycleEvent::getPortKind() const
{
    return kind_;
}


/**
 * @brief PortLifecycleEvent::getPort
 * @return
 */
const AggServerResponse::Port& PortLifecycleEvent::getPort() const
{
    return port_;
}


/**
 * @brief PortLifecycleEvent::getType
 * @return
 */
const AggServerResponse::LifecycleType& PortLifecycleEvent::getType() const
{
    return type_;
}


/**
 * @brief PortLifecycleEvent::getID
 * @return
 */
QString PortLifecycleEvent::getID() const
{
    return port_.graphml_id;
}


/**
 * @brief PortLifecycleEvent::getKind
 * @return
 */
TIMELINE_DATA_KIND PortLifecycleEvent::getKind() const
{
    return TIMELINE_DATA_KIND::PORT_LIFECYCLE;
}
