#include "portlifecycleeventseries.h"
#include <stdexcept>
/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param ID
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID , MEDEA::ChartDataKind::PORT_LIFECYCLE, parent) {}

/**
 * @brief PortLifecycleEventSeries::addEvent
 * @param event
 * @throws std::invalid_argument
 */
void PortLifecycleEventSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("PortLifecycleEventSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::PORT_LIFECYCLE) {
        throw std::invalid_argument("PortLifecycleEventSeries::addEvent - Invalid event kind.");
    }
    if (!contains(event)) {
        addEventToList(*event);
    }
}