#include "portlifecycleeventseries.h"

#include <QDateTime>
#include <QDebug>


/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param path
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(QString path, QObject* parent)
    : MEDEA::EventSeries(parent, TIMELINE_DATA_KIND::PORT_LIFECYCLE)
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
QString PortLifecycleEventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat)
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
    } else {
        // display upto the first 10 events
        QString hoveredData;
        QTextStream stream(&hoveredData);
        int displayCount = qMin(count, numberOfItemsToDisplay);
        for (int i = 0; i < displayCount; i++) {
            auto event = (PortLifecycleEvent*)(*current);
            stream << getTypeString(event->getType()) + " - "
                      + QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString(displayFormat) + "\n";
            current++;
        }
        if (count > 10) {
            stream << "... (more omitted)";
        }
        return hoveredData.trimmed();
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
