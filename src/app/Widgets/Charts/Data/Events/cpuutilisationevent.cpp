#include "cpuutilisationevent.h"


/**
 * @brief CPUUtilisationEvent::CPUUtilisationEvent
 * @param hostname
 * @param utilisation
 * @param time
 * @param parent
 */
CPUUtilisationEvent::CPUUtilisationEvent(const QString& hostname,
                                         double utilisation,
                                         qint64 time,
                                         QObject *parent)
    : MEDEA::Event(MEDEA::ChartDataKind::CPU_UTILISATION, time, hostname, parent),
      hostname_(hostname),
      utilisation_(utilisation) {}


/**
 * @brief CPUUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString CPUUtilisationEvent::toString(const QString &dateTimeFormat) const
{
    return "Host: " + hostname_ + "\n" +
           "Utilisation: " + QString::number(utilisation_ * 100) + "%\n" +
           "At: " + getDateTimeString(dateTimeFormat) + "\n\n";
}


/**
 * @brief CPUUtilisationEvent::getID
 * @return
 */
const QString& CPUUtilisationEvent::getID() const
{
    return hostname_;
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
 * @brief CPUUtilisationEvent::getUtilisation
 * @return
 */
double CPUUtilisationEvent::getUtilisation() const
{
    return utilisation_;
}
