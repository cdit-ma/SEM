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
    if (event->getKind() == MEDEA::ChartDataKind::CPU_UTILISATION) {
        auto utilisation = ((CPUUtilisationEvent*)event)->getUtilisation();
        if (utilisation < minUtilisation_) {
            minUtilisation_ = utilisation;
            emit minYValueChanged(utilisation);
        }
        if (utilisation > maxUtilisation_) {
            maxUtilisation_ = utilisation;
            emit maxYValueChanged(utilisation);
        }
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
