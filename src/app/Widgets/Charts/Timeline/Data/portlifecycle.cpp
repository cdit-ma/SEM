#include "portlifecycle.h"


/**
 * @brief PortLifeCycle::PortLifeCycle
 * @param parent
 */
PortLifeCycle::PortLifeCycle(Port port, LifeCycleType type, quint64 time, QObject* parent)
    : QObject(parent)
{
    port_ = port;
    type_ = type;
    time_ = time;
}


/**
 * @brief PortLifeCycle::PortLifeCycle
 * @param type
 * @param time
 * @param parent
 */
PortLifeCycle::PortLifeCycle(LifeCycleType type, quint64 time, QObject *parent)
    : QObject(parent)
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
}


/**
 * @brief PortLifeCycle::getPort
 * @return
 */
Port PortLifeCycle::getPort()
{
    return port_;
}


/**
 * @brief PortLifeCycle::getType
 * @return
 */
LifeCycleType PortLifeCycle::getType()
{
    return type_;
}


/**
 * @brief PortLifeCycle::getTime
 * @return
 */
quint64 PortLifeCycle::getTime()
{
    return time_;
}
