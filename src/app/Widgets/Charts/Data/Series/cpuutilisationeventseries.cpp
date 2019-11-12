#include "cpuutilisationeventseries.h"

/**
 * @brief CPUUtilisationEventSeries::CPUUtilisationEventSeries
 * @param ID
 * @param parent
 */
CPUUtilisationEventSeries::CPUUtilisationEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::CPU_UTILISATION, parent) {}


/**
 * @brief CPUUtilisationEventSeries::addEvent
 * @param event
 * @throws std::invalid_argument
 */
void CPUUtilisationEventSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("CPUUtilisationEventSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::CPU_UTILISATION) {
        throw std::invalid_argument("CPUUtilisationEventSeries::addEvent - Invalid event kind.");
    }

    if (!events_.contains(event)) {
        auto utilisation = qobject_cast<CPUUtilisationEvent*>(event)->getUtilisation();
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
 * @brief CPUUtilisationEventSeries::getMaxUtilisation
 * @return
 */
double CPUUtilisationEventSeries::getMaxUtilisation() const
{
    return maxUtilisation_;
}
