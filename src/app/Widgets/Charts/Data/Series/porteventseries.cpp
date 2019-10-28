#include "porteventseries.h"
#include "../Events/portevent.h"

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
 */
void PortEventSeries::addEvent(MEDEA::Event* event)
{
    if (event->getKind() != MEDEA::ChartDataKind::PORT_EVENT) {
        qCritical("PortEventSeries::addEvent - Cannot add event due to a mismatch of type.");
        return;
        // TODO - Replace above with the following statement for all the event series classes
        //throw std::invalid_argument("PortEventSeries::addEvent - Cannot add event due to a mismatch of type.");
    }
    MEDEA::EventSeries::addEvent(event);
}
