#include "portlifecycleeventseries.h"
#include "../Events/portlifecycleevent.h"

/**
 * @brief PortLifecycleEventSeries::PortLifecycleEventSeries
 * @param ID
 * @param parent
 */
PortLifecycleEventSeries::PortLifecycleEventSeries(QString ID, QObject* parent)
    : MEDEA::EventSeries(ID, TIMELINE_DATA_KIND::PORT_LIFECYCLE, parent) {}


/**
 * @brief PortLifecycleEventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
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
    if (count <= 0)
        return "";

    QString hoveredData;
    QTextStream stream(&hoveredData);
    numberOfItemsToDisplay = qMin(count, numberOfItemsToDisplay);

    for (int i = 0; i < numberOfItemsToDisplay; i++) {
        auto event = (PortLifecycleEvent*)(*current);
        stream << getTypeString(event->getType()) + " - "
                  + QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString(displayFormat) + "\n";
        current++;
    }
    if (count > numberOfItemsToDisplay)
        stream << "... (more omitted)";

    return hoveredData.trimmed();
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
