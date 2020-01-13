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
        auto delta_bytes_sent = qobject_cast<NetworkUtilisationEvent*>(event)->getDeltaBytesSent();
        auto delta_bytes_received = qobject_cast<NetworkUtilisationEvent*>(event)->getDeltaBytesReceived();
        auto min_delta_bytes = qMin(delta_bytes_sent, delta_bytes_received);
        auto max_delta_bytes = qMax(delta_bytes_sent, delta_bytes_received);
        if (min_delta_bytes > 0 && min_delta_bytes < min_) {
            min_ = min_delta_bytes;
            emit minYValueChanged(min_delta_bytes);
        }
        if (max_delta_bytes > max_) {
            max_ = max_delta_bytes;
            emit maxYValueChanged(max_delta_bytes);
        }
        addEventToList(*event);
    }
}