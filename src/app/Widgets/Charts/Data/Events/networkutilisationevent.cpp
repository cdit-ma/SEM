#include "networkutilisationevent.h"


/**
 * @brief NetworkUtilisationEvent::NetworkUtilisationEvent
 * @param hostname
 * @param utilisation
 * @param time
 * @param parent
 */
NetworkUtilisationEvent::NetworkUtilisationEvent(const QString& hostname,
                                               const QString& interface_mac_addr,
                                               qint64 delta_packets_sent,
                                               qint64 delta_packets_received,
                                               qint64 delta_bytes_sent,
                                               qint64 delta_bytes_received,
                                               qint64 time,
                                               QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::NETWORK_UTILISATION, time, hostname, parent),
      series_id_(hostname + interface_mac_addr),
      id_(series_id_ + QString::number(time)),
      hostname_(hostname),
      delta_packets_sent_(delta_packets_sent),
      delta_packets_received_(delta_packets_received),
      delta_bytes_sent_(delta_bytes_sent),
      delta_bytes_received_(delta_bytes_received) {}


/**
 * @brief NetworkUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString NetworkUtilisationEvent::toString(const QString& dateTimeFormat) const
{
    return  "Host: " + hostname_ + "\n" +
            "Delta Bytes Sent: " + QString::number(delta_bytes_sent_) + "\n" +
            "Delta Bytes Received: " + QString::number(delta_bytes_received_) + "\n" +
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
 * @brief NetworkUtilisationEvent::getInterfaceMacAddress
 * @return
 */
const QString& NetworkUtilisationEvent::getInterfaceMacAddress() const
{
    return interface_mac_address_;
}


/**
 * @brief NetworkUtilisationEvent::getDeltaPacketsSent
 * @return
 */
qint64 NetworkUtilisationEvent::getDeltaPacketsSent() const
{
    return delta_packets_sent_;
}


/**
 * @brief NetworkUtilisationEvent::getDeltaPacketsReceived
 * @return
 */
qint64 NetworkUtilisationEvent::getDeltaPacketsReceived() const
{
    return delta_bytes_received_;
}


/**
 * @brief NetworkUtilisationEvent::getDeltaBytesSent
 * @return
 */
qint64 NetworkUtilisationEvent::getDeltaBytesSent() const
{
    return delta_bytes_sent_;
}


/**
 * @brief NetworkUtilisationEvent::getDeltaBytesReceived
 * @return
 */
qint64 NetworkUtilisationEvent::getDeltaBytesReceived() const
{
    return delta_bytes_received_;
}