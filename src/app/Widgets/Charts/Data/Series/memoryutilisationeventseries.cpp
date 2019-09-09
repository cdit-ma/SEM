#include "memoryutilisationeventseries.h"
#include "../Events/memoryutilisationevent.h"

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
 */
void MemoryUtilisationEventSeries::addEvent(MEDEA::Event *event)
{
    if (event->getKind() != MEDEA::ChartDataKind::MEMORY_UTILISATION) {
        qCritical("MemoryUtilisationEventSeries::addEvent - Cannot add event due to a mismatch of type.");
        return;
    }

    auto utilisation = qobject_cast<MemoryUtilisationEvent*>(event)->getUtilisation();
    if (utilisation < minUtilisation_) {
        minUtilisation_ = utilisation;
        emit minYValueChanged(utilisation);
    }
    if (utilisation > maxUtilisation_) {
        maxUtilisation_ = utilisation;
        emit maxYValueChanged(utilisation);
    }

    MEDEA::EventSeries::addEvent(event);
}


/**
 * @brief MemoryUtilisationEventSeries::getMaxUtilisation
 * @return
 */
double MemoryUtilisationEventSeries::getMaxUtilisation() const
{
    return maxUtilisation_;
}
