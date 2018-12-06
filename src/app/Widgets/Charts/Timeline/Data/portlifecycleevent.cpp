#include "portlifecycleevent.h"


/**
<<<<<<< HEAD
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
=======
 * @brief PortLifeCycle::PortLifeCycle
 * @param parent
 */
PortLifeCycle::PortLifeCycle(Port port, LifeCycleType type, qint64 time, QObject* parent)
    : QObject(parent)
{
    port_ = port;
    type_ = type;
    time_ = time;
}

PortLifeCycle::PortLifeCycle(LifeCycleType type, qint64 time, QObject *parent)
{
    Port testPort;
    testPort.ID = 0;
    testPort.kind = Port::Kind::PUBLISHER;
    testPort.path = "path";
    testPort.name = "testPort";
    testPort.middleware = "middleware";

    port_ = testPort;
    type_ = type;
    time_ = time;
>>>>>>> feature-hub
}


/**
<<<<<<< HEAD
 * @brief PortLifecycleEvent::getType
 * @return
 */
const LifecycleType PortLifecycleEvent::getType()
{
    return type_;
=======
 * @brief PortLifeCycle::getPort
 * @return
 */
Port PortLifeCycle::getPort()
{
    return port_;
>>>>>>> feature-hub
}


/**
<<<<<<< HEAD
 * @brief PortLifecycleEvent::getPortGraphmlID
 * @return
 */
const QString PortLifecycleEvent::getPortGraphmlID()
{
    return port_.graphml_id;
=======
 * @brief PortLifeCycle::getType
 * @return
 */
LifeCycleType PortLifeCycle::getType()
{
    return type_;
>>>>>>> feature-hub
}


/**
<<<<<<< HEAD
 * @brief PortLifecycleEvent::getPortPath
 * @return
 */
const QString PortLifecycleEvent::getPortPath()
{
    return port_.path;
=======
 * @brief PortLifeCycle::getTime
 * @return
 */
qint64 PortLifeCycle::getTime()
{
    return time_;
>>>>>>> feature-hub
}
