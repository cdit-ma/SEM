#include "portlifecycleeventseries.h"


int PortLifecycleEventSeries::series_ID = 0;

/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(QObject* parent)
    : MEDEA::EventSeries(parent)
{
    ID_ = series_ID++;
}


/**
 * @brief PortLifecycleEventSeries::getID
 * @return
 */
int PortLifecycleEventSeries::getID()
{
    return -1; //ID_;
}


/**
 * @brief PortLifecycleEventSeries::addPortEvent
 * @param event
 */
void PortLifecycleEventSeries::addPortEvent(PortLifecycleEvent* event)
{
    portEvents_.append(event);
}


/**
 * @brief PortLifecycleEventSeries::addPortEvents
 * @param events
 */
void PortLifecycleEventSeries::addPortEvents(QList<PortLifecycleEvent*> events)
{
    portEvents_.append(events);
}


/**
 * @brief PortLifecycleEventSeries::getConstPortEvents
 * @return
 */
const QList<PortLifecycleEvent*> &PortLifecycleEventSeries::getConstPortEvents()
{
    return portEvents_;
}

