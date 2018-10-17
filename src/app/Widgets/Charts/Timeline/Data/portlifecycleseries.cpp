#include "portlifecycleseries.h"


int PortLifeCycleSeries::series_ID = 0;

/**
 * @brief PortLifeCycleSeries::PortLifeCycleSeries
 * @param parent
 */
PortLifeCycleSeries::PortLifeCycleSeries(QObject* parent)
    : QObject(parent)
{
    ID_ = series_ID++;
}


/**
 * @brief PortLifeCycleSeries::getID
 * @return
 */
int PortLifeCycleSeries::getID()
{
    return -1; //ID_;
}


/**
 * @brief PortLifeCycleSeries::addPortEvent
 * @param event
 */
void PortLifeCycleSeries::addPortEvent(PortLifeCycle* event)
{
    portEvents_.append(event);
}


/**
 * @brief PortLifeCycleSeries::addPortEvents
 * @param events
 */
void PortLifeCycleSeries::addPortEvents(QList<PortLifeCycle*> events)
{
    portEvents_.append(events);
}


/**
 * @brief PortLifeCycleSeries::getConstPortEvents
 * @return
 */
const QList<PortLifeCycle*> &PortLifeCycleSeries::getConstPortEvents()
{
    return portEvents_;
}

