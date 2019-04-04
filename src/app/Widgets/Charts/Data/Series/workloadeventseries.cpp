#include "workloadeventseries.h"
#include "../Events/workloadevent.h"

/**
 * @brief WorkloadEventSeries::WorkloadEventSeries
 * @param ID
 * @param parent
 */
WorkloadEventSeries::WorkloadEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::WORKLOAD, parent) {}
