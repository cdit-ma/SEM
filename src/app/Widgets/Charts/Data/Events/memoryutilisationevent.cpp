#include "memoryutilisationevent.h"


/**
 * @brief MemoryUtilisationEvent::MemoryUtilisationEvent
 * @param hostname
 * @param utilisation
 * @param time
 * @param parent
 */
MemoryUtilisationEvent::MemoryUtilisationEvent(QString hostname, double utilisation, qint64 time, QObject *parent)
    : MEDEA::Event(time, hostname, parent)
{
    hostname_ = hostname;
    utilisation_ = utilisation;
}


/**
 * @brief MemoryUtilisationEvent::getUtilisation
 * @return
 */
const double& MemoryUtilisationEvent::getUtilisation() const
{
    return utilisation_;
}


/**
 * @brief MemoryUtilisationEvent::getHostname
 * @return
 */
const QString& MemoryUtilisationEvent::getHostname() const
{
    return hostname_;
}


/**
 * @brief MemoryUtilisationEvent::getID
 * @return
 */
QString MemoryUtilisationEvent::getID() const
{
    return hostname_;
}


/**
 * @brief MemoryUtilisationEvent::getKind
 * @return
 */
TIMELINE_EVENT_KIND MemoryUtilisationEvent::getKind() const
{
    return TIMELINE_EVENT_KIND::MEMORY_UTILISATION;
}
