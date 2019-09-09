#include "workloadeventseries.h"
#include "../Events/workloadevent.h"

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
 */
void WorkloadEventSeries::addEvent(MEDEA::Event* event)
{
    if (event->getKind() != MEDEA::ChartDataKind::WORKLOAD) {
        qCritical("WorkloadEventSeries::addEvent - Cannot add event due to a mismatch of type.");
        return;
    }

    MEDEA::EventSeries::addEvent(event);
}
