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
                                               qint64 packets_sent,
                                               qint64 packets_received,
                                               qint64 bytes_sent,
                                               qint64 bytes_received,
                                               qint64 time,
                                               QObject* parent)
    : MEDEA::Event(MEDEA::ChartDataKind::NETWORK_UTILISATION, time, parent),
      id_(hostname + interface_mac_addr + QString::number(time)),
      hostname_(hostname),
      packets_sent_(packets_sent),
      packets_received_(packets_received),
      bytes_sent_(bytes_sent),
      bytes_received_(bytes_received) {}


/**
 * @brief NetworkUtilisationEvent::toString
 * @param dateTimeFormat
 * @return
 */
QString NetworkUtilisationEvent::toString(const QString& dateTimeFormat) const
{
    return "Host: " + hostname_ + "\n" +
           "Bytes Sent: " + QString::number(bytes_sent_) + "\n" +
           "Bytes Received: " + QString::number(bytes_received_) + "\n" +
           "At: " + getDateTimeString(dateTimeFormat) + "\n\n";
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
 * @brief NetworkUtilisationEvent::getPacketsSent
 * @return
 */
qint64 NetworkUtilisationEvent::getPacketsSent() const
{
    return packets_sent_;
}

/**
 * @brief NetworkUtilisationEvent::getPacketsReceived
 * @return
 */
qint64 NetworkUtilisationEvent::getPacketsReceived() const
{
    return bytes_received_;
}

/**
 * @brief NetworkUtilisationEvent::getBytesSent
 * @return
 */
qint64 NetworkUtilisationEvent::getBytesSent() const
{
    return bytes_sent_;
}

/**
 * @brief NetworkUtilisationEvent::getBytesReceived
 * @return
 */
qint64 NetworkUtilisationEvent::getBytesReceived() const
{
    return bytes_received_;
}