#include "workloadeventseries.h"
#include <stdexcept>
/**
 * @brief WorkloadEventSeries::WorkloadEventSeries
 * @param ID
 * @param parent
 */
WorkloadEventSeries::WorkloadEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::WORKLOAD, parent) {}

/**
 * @brief WorkloadEventSeries::addEvent
 * @param event
 * @throws std::invalid_argument
 */
void WorkloadEventSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("WorkloadEventSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::WORKLOAD) {
        throw std::invalid_argument("WorkloadEventSeries::addEvent - Invalid event kind.");
    }
    if (!contains(event)) {
        addEventToList(*event);
    }
}