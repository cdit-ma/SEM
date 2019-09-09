#include "cpuutilisationeventseries.h"
#include "../Events/cpuutilisationevent.h"

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
 */
void CPUUtilisationEventSeries::addEvent(MEDEA::Event* event)
{
    if (event->getKind() != MEDEA::ChartDataKind::CPU_UTILISATION) {
        qCritical("CPUUtilisationEventSeries::addEvent - Cannot add event due to a mismatch of type.");
        return;
    }

    auto utilisation = qobject_cast<CPUUtilisationEvent*>(event)->getUtilisation();
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
 * @brief CPUUtilisationEventSeries::getMaxUtilisation
 * @return
 */
double CPUUtilisationEventSeries::getMaxUtilisation() const
{
    return maxUtilisation_;
}
