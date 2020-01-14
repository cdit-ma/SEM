#include "networkutilisationeventseries.h"

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
        auto delta_bytes_sent = qobject_cast<NetworkUtilisationEvent*>(event)->getDeltaBytesSent();
        auto delta_bytes_received = qobject_cast<NetworkUtilisationEvent*>(event)->getDeltaBytesReceived();

        // Send a signal to set the min y value for the charts
        if (isEmpty()) {
            emit minYValueChanged(qMin(delta_bytes_sent, delta_bytes_received));
        }

        total_sent_ += delta_bytes_sent;
        total_received_ += delta_bytes_received;

        // Calculate the new max y value
        auto total_max = qMax(total_sent_, total_received_);
        if (total_max > max_) {
            max_ = total_max;
            emit maxYValueChanged(max_);
        }
        addEventToList(*event);
    }
}