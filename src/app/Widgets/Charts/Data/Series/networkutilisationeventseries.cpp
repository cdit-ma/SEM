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
        auto utilisation = qobject_cast<NetworkUtilisationEvent*>(event)->getUtilisation();
        if (utilisation < minUtilisation_) {
            minUtilisation_ = utilisation;
            emit minYValueChanged(utilisation);
        }
        if (utilisation > maxUtilisation_) {
            maxUtilisation_ = utilisation;
            emit maxYValueChanged(utilisation);
        }
        addEventToList(*event);
    }
}


/**
 * @brief NetworkUtilisationEventSeries::getMaxUtilisation
 * @return
 */
double NetworkUtilisationEventSeries::getMaxUtilisation() const
{
    return maxUtilisation_;
}
