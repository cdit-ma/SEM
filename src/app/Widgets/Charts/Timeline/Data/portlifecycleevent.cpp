#include "portlifecycleevent.h"


/**
 * @brief PortLifecycleEvent::PortLifecycleEvent
 * @param parent
 */
PortLifecycleEvent::PortLifecycleEvent(Port port, LifecycleType type, quint64 time, QObject* parent)
    : QObject(parent)
{
    port_ = port;
    type_ = type;
    time_ = time;
}


/**
 * @brief PortLifecycleEvent::PortLifecycleEvent
 * @param type
 * @param time
 * @param parent
 */
PortLifecycleEvent::PortLifecycleEvent(LifecycleType type, quint64 time, QObject* parent)
    : QObject(parent)
{
    Port testPort;
    //testPort.ID = 0;
    //testPort.kind = Port::Kind::PUBLISHER;
    testPort.path = "path";
    testPort.name = "testPort";
    testPort.middleware = "middleware";

    port_ = testPort;
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
quint64 PortLifecycleEvent::getTime()
{
    return time_;
}
