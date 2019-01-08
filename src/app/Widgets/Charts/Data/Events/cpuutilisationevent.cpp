#include "cpuutilisationevent.h"


/**
 * @brief CPUUtilisationEvent::CPUUtilisationEvent
 * @param hostname
 * @param utilisation
 * @param time
 * @param parent
 */
CPUUtilisationEvent::CPUUtilisationEvent(QString hostname, double utilisation, qint64 time, QObject *parent)
    : MEDEA::Event(time, hostname, parent)
{
    hostname_ = hostname;
    utilisation_ = utilisation;
}


/**
 * @brief CPUUtilisationEvent::getUtilisation
 * @return
 */
const double& CPUUtilisationEvent::getUtilisation() const
{
    return utilisation_;
}


/**
 * @brief CPUUtilisationEvent::getHostname
 * @return
 */
const QString& CPUUtilisationEvent::getHostname() const
{
    return hostname_;
}


/**
 * @brief CPUUtilisationEvent::getID
 * @return
 */
QString CPUUtilisationEvent::getID() const
{
    return hostname_;
}


/**
 * @brief CPUUtilisationEvent::getKind
 * @return
 */
TIMELINE_EVENT_KIND CPUUtilisationEvent::getKind() const
{
    return TIMELINE_EVENT_KIND::CPU_UTILISATION;
}
