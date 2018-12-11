#include "cpuutilisationevent.h"


/**
 * @brief CPUUtilisationEvent::CPUUtilisationEvent
 * @param node
 * @param utilisation
 * @param time
 * @param parent
 */
CPUUtilisationEvent::CPUUtilisationEvent(Node node, double utilisation, qint64 time, QObject *parent)
    : MEDEA::Event(time, node.hostname, parent)
{
    node_ = node;
    utilisation_ = utilisation;
}


/**
 * @brief CPUUtilisationEvent::getUtilisation
 * @return
 */
double CPUUtilisationEvent::getUtilisation() const
{
    return utilisation_;
}


/**
 * @brief CPUUtilisationEvent::getHostname
 * @return
 */
QString CPUUtilisationEvent::getHostname() const
{
    return node_.hostname;
}


/**
 * @brief CPUUtilisationEvent::getIp
 * @return
 */
QString CPUUtilisationEvent::getIp() const
{
    return node_.ip;
}
