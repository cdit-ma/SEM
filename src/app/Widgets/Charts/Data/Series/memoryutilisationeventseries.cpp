#include "memoryutilisationeventseries.h"

/**
 * @brief MemoryUtilisationEventSeries::MemoryUtilisationEventSeries
 * @param ID
 * @param parent
 */
MemoryUtilisationEventSeries::MemoryUtilisationEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::MEMORY_UTILISATION, parent) {}


/**
 * @brief MemoryUtilisationEventSeries::addEvent
 * @param event
 * @throws std::invalid_argument
 */
void MemoryUtilisationEventSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("MemoryUtilisationEventSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::MEMORY_UTILISATION) {
        throw std::invalid_argument("MemoryUtilisationEventSeries::addEvent - Invalid event kind.");
    }
    if (!contains(event)) {
        auto utilisation = qobject_cast<MemoryUtilisationEvent*>(event)->getUtilisation();
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