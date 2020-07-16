#include "networkutilisationeventseries.h"
#include <cmath>
#include <stdexcept>
/**
 * @brief NetworkUtilisationEventSeries::NetworkUtilisationEventSeries
 * @param ID
 * @param parent
 */
NetworkUtilisationEventSeries::NetworkUtilisationEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::NETWORK_UTILISATION, parent) {}


/**
 * @brief NetworkUtilisationEventSeries::addEvent
 * @param event
 * @throws std::invalid_argument
 */
void NetworkUtilisationEventSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("NetworkUtilisationEventSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::NETWORK_UTILISATION) {
        throw std::invalid_argument("NetworkUtilisationEventSeries::addEvent - Invalid event kind.");
    }
    if (!contains(event)) {

        // NOTE - We are currently only checking and displaying the bytes sent/received
        auto bytes_sent = qobject_cast<NetworkUtilisationEvent *>(event)->getBytesSent();
        auto bytes_received = qobject_cast<NetworkUtilisationEvent *>(event)->getBytesReceived();

        // Send a signal to set the min y value for the charts
        if (isEmpty()) {
            minUtilisation_ = qMin(bytes_sent, bytes_received);
            emit minYValueChanged(minUtilisation_);
        }

        maxUtilisation_ = qMax(bytes_sent, bytes_received);
        emit maxYValueChanged(maxUtilisation_);
        addEventToList(*event);
    }
}


/**
 * @brief NetworkUtilisationEventSeries::getByteString
 * @param bytes
 * @return
 */
QString NetworkUtilisationEventSeries::getByteString(qint64 bytes)
{
    auto kb = bytes / 1024.0;
    auto mb = bytes / 1048576.0;
    auto gb = bytes / 1073741824.0;
    auto tb = bytes / 1099511627776.0;

    double converted_bytes = bytes;
    QString suffix = "B";

    if (tb >= 1) {
        converted_bytes = trimDecimalPlaces(tb, 2);
        suffix = "TB";
    } else if (gb >= 1) {
        converted_bytes = trimDecimalPlaces(gb, 2);
        suffix = "GB";
    } else if (mb >= 1) {
        converted_bytes = trimDecimalPlaces(mb, 2);
        suffix = "MB";
    } else if (kb >= 1) {
        converted_bytes = trimDecimalPlaces(kb, 2);
        suffix = "KB";
    }

    return QString::number(converted_bytes) + suffix;
}


/**
 * @brief NetworkUtilisationEventSeries
 * @param number
 * @param desired_decimal_places
 * @return
 */
double NetworkUtilisationEventSeries::trimDecimalPlaces(double number, int desired_decimal_places)
{
    double multiplier = 10.0;
    for (int i = 1; i < desired_decimal_places; i++) {
        multiplier *= 10;
    }
    return round(number * multiplier) / multiplier;
}


/**
 * @brief NetworkUtilisationEventSeries::rightPad
 * @param str
 * @param intended_length
 * @param pad_char
 */
void NetworkUtilisationEventSeries::rightPad(QString &str, int intended_length, char pad_char)
{
    while (str.length() < intended_length) {
        str += pad_char;
    }
}

/**
 * @brief NetworkUtilisationEventSeries::getMinUtilisation
 * @return
 */
double NetworkUtilisationEventSeries::getMinUtilisation() const
{
    return minUtilisation_;
}

/**
 * @brief NetworkUtilisationEventSeries::getMaxUtilisation
 * @return
 */
double NetworkUtilisationEventSeries::getMaxUtilisation() const
{
    return maxUtilisation_;
}