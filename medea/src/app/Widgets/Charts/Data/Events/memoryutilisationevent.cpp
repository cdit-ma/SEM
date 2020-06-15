#include "memoryutilisationevent.h"

/**
 * @brief MemoryUtilisationEvent::MemoryUtilisationEvent
 * @param hostname
 * @param utilisation
 * @param time
 * @param parent
 */
MemoryUtilisationEvent::MemoryUtilisationEvent(const QString& hostname,
                                               double utilisation,
                                               qint64 time,
                                               QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::MEMORY_UTILISATION, time, parent),
      id_(hostname + QString::number(time)),
      hostname_(hostname),
      utilisation_(utilisation) {}

/**
 * @brief MemoryUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString MemoryUtilisationEvent::toString(const QString& dateTimeFormat) const
{
	return "Host: " + hostname_ + "\n" +
		   "Utilisation: " + QString::number(utilisation_ * 100) + "%\n" +
		   "At: " + getDateTimeString(dateTimeFormat) + "\n\n";
}

/**
 * @brief MemoryUtilisationEvent::getID
 * @return
 */
const QString& MemoryUtilisationEvent::getID() const
{
    return id_;
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
 * @brief MemoryUtilisationEvent::getUtilisation
 * @return
 */
double MemoryUtilisationEvent::getUtilisation() const
{
    return utilisation_;
}