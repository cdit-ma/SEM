#include "workloadeventseries.h"
#include "../Events/workloadevent.h"

/**
 * @brief WorkloadEventSeries::WorkloadEventSeries
 * @param ID
 * @param parent
 */
WorkloadEventSeries::WorkloadEventSeries(QString ID, QObject* parent)
    : MEDEA::EventSeries(ID, TIMELINE_DATA_KIND::WORKLOAD, parent) {}


/**
 * @brief WorkloadEventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString WorkloadEventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat)
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
        auto event = (WorkloadEvent*)(*current);
        stream << "[" + event->getFunctionName() + "] - "
                  + QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString(displayFormat) + "\n";
        if (!event->getArgs().isEmpty()) {
            stream << event->getArgs().trimmed() + "\n\n";
        } else {
            stream << "\n";
        }
        current++;
    }
    if (count > numberOfItemsToDisplay)
        stream << "... (more omitted)";

    return hoveredData.trimmed();
}
