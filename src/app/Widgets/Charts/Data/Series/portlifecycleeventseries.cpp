#include "portlifecycleeventseries.h"

#include <QDateTime>
#include <QDebug>


/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param path
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(QString path, QObject* parent)
    : MEDEA::EventSeries(parent)
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
