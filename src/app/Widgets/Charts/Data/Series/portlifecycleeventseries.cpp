#include "portlifecycleeventseries.h"
#include "../Events/portlifecycleevent.h"

/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param ID
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::PORT_LIFECYCLE, parent) {}


/**
 * @brief PortLifecycleEventSeries::addEvent
 * @param event
 */
void PortLifecycleEventSeries::addEvent(MEDEA::Event* event)
{
    if (event->getKind() != MEDEA::ChartDataKind::PORT_LIFECYCLE) {
        qCritical("PortLifecycleEventSeries::addEvent - Cannot add event due to a mismatch of type.");
        return;
    }

    MEDEA::EventSeries::addEvent(event);
}
