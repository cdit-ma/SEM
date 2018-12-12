#include "portlifecycleeventseries.h"

<<<<<<< HEAD
#include <QDateTime>
#include <QDebug>


/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param path
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(QString path, QObject* parent)
    : QObject(parent)
{
    port_path = path;
}


/**
 * @brief PortLifecycleEventSeries::getPortPath
 * @return
 */
QString PortLifecycleEventSeries::getPortPath()
{
    return port_path;
}
=======

int PortLifeCycleSeries::series_ID = 0;

/**
 * @brief PortLifeCycleSeries::PortLifeCycleSeries
 * @param parent
 */
PortLifeCycleSeries::PortLifeCycleSeries(QObject* parent)
    : BaseSeries(parent)
{
    ID_ = series_ID++;
}

/*
PortLifeCycleSeries::PortLifeCycleSeries(int ID, QObject *parent)
    : QObject(parent)
{
    ID_ = ID;
}
*/


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

>>>>>>> feature-hub
