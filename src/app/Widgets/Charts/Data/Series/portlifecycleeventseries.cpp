#include "portlifecycleeventseries.h"
#include "../Events/portlifecycleevent.h"

/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param ID
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::PORT_LIFECYCLE, parent) {}
