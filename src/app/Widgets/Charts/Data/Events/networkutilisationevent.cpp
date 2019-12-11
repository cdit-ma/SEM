#include "networkutilisationevent.h"


/**
 * @brief NetworkUtilisationEvent::NetworkUtilisationEvent
 * @param hostname
 * @param utilisation
 * @param time
 * @param parent
 */
NetworkUtilisationEvent::NetworkUtilisationEvent(const QString& hostname,
                                               double utilisation,
                                               qint64 time,
                                               QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::NETWORK_UTILISATION, time, hostname, parent),
      series_id_(hostname),
      id_(hostname + QString::number(time)),
      hostname_(hostname),
      utilisation_(utilisation) {}


/**
 * @brief NetworkUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString NetworkUtilisationEvent::toString(const QString& dateTimeFormat) const
{
    return "Host: " + hostname_ + "\n" +
            "Utilisation: " + QString::number(utilisation_ * 100) + "%\n" +
            "At: " + getDateTimeString(dateTimeFormat) + "\n\n";
}


/**
 * @brief NetworkUtilisationEvent::getSeriesID
 * @return
 */
const QString& NetworkUtilisationEvent::getSeriesID() const
{
    return series_id_;
}


/**
 * @brief NetworkUtilisationEvent::getID
 * @return
 */
const QString& NetworkUtilisationEvent::getID() const
{
    return id_;
}


/**
 * @brief NetworkUtilisationEvent::getHostname
 * @return
 */
const QString& NetworkUtilisationEvent::getHostname() const
{
    return hostname_;
}


/**
 * @brief NetworkUtilisationEvent::getUtilisation
 * @return
 */
double NetworkUtilisationEvent::getUtilisation() const
{
    return utilisation_;
}
