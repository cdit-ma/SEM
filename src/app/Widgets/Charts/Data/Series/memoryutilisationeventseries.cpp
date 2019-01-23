#include "memoryutilisationeventseries.h"
#include "../Events/memoryutilisationevent.h"

/**
 * @brief MemoryUtilisationEventSeries::MemoryUtilisationEventSeries
 * @param ID
 * @param parent
 */
MemoryUtilisationEventSeries::MemoryUtilisationEventSeries(QString ID, QObject* parent)
    : MEDEA::EventSeries(ID, TIMELINE_DATA_KIND::MEMORY_UTILISATION, parent) {}


/**
 * @brief MemoryUtilisationEventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString MemoryUtilisationEventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat)
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
        auto event = (MemoryUtilisationEvent*)(*current);
        stream << "Host: " << event->getHostname() << "\n"
               << "Utilisation: " << (event->getUtilisation() * 100) << "%\n"
               << "At: " << QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("hh:mm:ss:zzz") << "\n\n";
        current++;
    }
    if (count > numberOfItemsToDisplay)
        stream << "... (more omitted)";

    return hoveredData.trimmed();
}
