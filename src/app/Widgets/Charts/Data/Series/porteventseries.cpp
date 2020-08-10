#include "porteventseries.h"

/**
 * @brief PortEventSeries::PortEventSeries
 * @param ID
 * @param parent
 */
PortEventSeries::PortEventSeries(const QString& ID, QObject* parent)
    : MEDEA::EventSeries(ID, MEDEA::ChartDataKind::PORT_EVENT, parent) {}

/**
 * @brief PortEventSeries::addEvent
 * @param event
 * @throws std::invalid_argument
 */
void PortEventSeries::addEvent(MEDEA::Event* event)
{
    if (event == nullptr) {
        throw std::invalid_argument("PortEventSeries::addEvent - Event parameter is null.");
    }
    if (event->getKind() != MEDEA::ChartDataKind::PORT_EVENT) {
        throw std::invalid_argument("PortEventSeries::addEvent - Invalid event kind.");
    }
    if (!contains(event)) {
        addEventToList(*event);
    }
}