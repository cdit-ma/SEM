#include "portlifecycleeventseries.h"

#include <QDateTime>
#include <QDebug>

int PortLifecycleEventSeries::series_ID = 0;

/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(QString path, QObject* parent)
    : QObject(parent)
{
    port_path = path;

    minTime_mu = QDateTime::currentMSecsSinceEpoch() * 1E3;
    maxTime_mu = 0;
}


/**
 * @brief PortLifecycleEventSeries::addPortEvent
 * @param event
 */
void PortLifecycleEventSeries::addPortEvent(PortLifecycleEvent* event)
{
    if (event) {

        /*if (portEvents_.contains(event))
            return;*/

        portEvents_.append(event);

        /*
        if (port_path == "top_level_assembly.1/") {
            qDebug() << "EVENTS#: " << portEvents_.count();
        }
        */

        // update the range
        auto eventTime = event->getTime();
        if (eventTime < minTime_mu)
            minTime_mu = eventTime;
        if (eventTime > maxTime_mu)
            maxTime_mu = eventTime;
    }
}


/**
 * @brief PortLifecycleEventSeries::addPortEvents
 * @param events
 */
void PortLifecycleEventSeries::addPortEvents(QList<PortLifecycleEvent*>& events)
{
    for (auto event : events) {
        addPortEvent(event);
    }
}


/**
 * @brief PortLifecycleEventSeries::clear
 */
void PortLifecycleEventSeries::clear()
{
    portEvents_.clear();
}


/**
 * @brief PortLifecycleEventSeries::getConstPortEvents
 * @return
 */
const QList<PortLifecycleEvent*>& PortLifecycleEventSeries::getConstPortEvents()
{
    return portEvents_;
}


/**
 * @brief PortLifecycleEventSeries::getPortPath
 * @return
 */
QString PortLifecycleEventSeries::getPortPath()
{
    return port_path;
}


/**
 * @brief PortLifecycleEventSeries::getRange
 * @return
 */
QPair<qint64, qint64> PortLifecycleEventSeries::getRange()
{
    return {minTime_mu, maxTime_mu};
}

