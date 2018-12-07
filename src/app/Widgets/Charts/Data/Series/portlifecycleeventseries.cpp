#include "portlifecycleeventseries.h"

#include <QDateTime>
#include <QDebug>


/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param path
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(QString path, QObject* parent)
    : MEDEA::EventSeries(parent, TIMELINE_SERIES_KIND::EVENT)
{
    port_path = path;
}


/**
 * @brief PortLifecycleEventSeries::getPortPath
 * @return
 */
const QString &PortLifecycleEventSeries::getPortPath() const
{
    return port_path;
}


/**
 * @brief PortLifecycleEventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param displayFormat
 * @return
 */
QString PortLifecycleEventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, QString displayFormat)
{
    const auto& data = getEvents();
    auto current = std::lower_bound(data.cbegin(), data.cend(), fromTimeMS, [](const MEDEA::Event* e, const qint64 &time) {
        return e->getTimeMS() < time;
    });
    auto upper = std::upper_bound(data.cbegin(), data.cend(), toTimeMS, [](const qint64 &time, const MEDEA::Event* e) {
        return time < e->getTimeMS();
    });

    int count = std::distance(current, upper);
    if (count <= 0) {
        return "";
    } else if (count == 1) {
        auto event = (PortLifecycleEvent*)(*current);
        if (event) {
            return /*"[" +*/ getTypeString(event->getType()) + " - " //+ "] - "
                    + QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("MMM d, hh:mm:ss:zzz");
        } else {
            return "";
        }
    } else {
        return QString::number(count);
    }
}


/**
 * @brief PortLifecycleEventSeries::getTypeString
 * @param type
 * @return
 */
QString PortLifecycleEventSeries::getTypeString(LifecycleType type)
{
    switch (type) {
    case LifecycleType::CONFIGURE:
        return "CONFIGURE";
    case LifecycleType::ACTIVATE:
        return "ACTIVATE";
    case LifecycleType::PASSIVATE:
        return "PASSIVATE";
    case LifecycleType::TERMINATE:
        return "TERMINATE";
    default:
        return "UNKNOWN";
    }
}
