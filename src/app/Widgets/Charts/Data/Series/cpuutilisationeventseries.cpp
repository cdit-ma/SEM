#include "cpuutilisationeventseries.h"
#include "../Events/cpuutilisationevent.h"

/**
 * @brief CPUUtilisationEventSeries::CPUUtilisationEventSeries
 * @param ID
 * @param parent
 */
CPUUtilisationEventSeries::CPUUtilisationEventSeries(QString ID, QObject* parent)
    : MEDEA::EventSeries(ID, TIMELINE_DATA_KIND::CPU_UTILISATION, parent) {}


/**
 * @brief CPUUtilisationEventSeries::addEvent
 * @param event
 */
void CPUUtilisationEventSeries::addEvent(MEDEA::Event* event)
{
    if (event->getKind() == TIMELINE_DATA_KIND::CPU_UTILISATION) {
        auto utilisation = ((CPUUtilisationEvent*)event)->getUtilisation();
        if (utilisation < minUtilisation_) {
            minUtilisation_ = utilisation;
            emit minYValueChanged(utilisation);
        }
        if (utilisation > maxUtilisation_) {
            maxUtilisation_ = utilisation;
            emit maxYValueChanged(utilisation);
        }
    }
    MEDEA::EventSeries::addEvent(event);
}


/**
 * @brief CPUUtilisationEventSeries::getMaxUtilisation
 * @return
 */
double CPUUtilisationEventSeries::getMaxUtilisation()
{
    return maxUtilisation_;
}


/**
 * @brief CPUUtilisationEventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString CPUUtilisationEventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat)
{
    const auto& data = getEvents();
    auto current = std::lower_bound(data.constBegin(), data.constEnd(), fromTimeMS, [](const MEDEA::Event* e, const qint64 &time) {
        return e->getTimeMS() < time;
    });
    auto upper = std::upper_bound(data.constBegin(), data.constEnd(), toTimeMS, [](const qint64 &time, const MEDEA::Event* e) {
        return time < e->getTimeMS();
    });

    /*int count = std::distance(current, upper);
    if (count <= 0)
        return "";*/

    if (current == data.constEnd())
        return "";

    QString hoveredData;
    QTextStream stream(&hoveredData);
    int count = std::distance(current, upper);
    numberOfItemsToDisplay = qMin(count, numberOfItemsToDisplay);
    numberOfItemsToDisplay = qMax(1, numberOfItemsToDisplay);

    for (int i = 0; i < numberOfItemsToDisplay; i++) {
        auto event = (CPUUtilisationEvent*)(*current);
        stream << "Host: " << event->getHostname() << "\n"
               << "Utilisation: " << (event->getUtilisation() * 100) << "%\n"
               << "At: " << QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("hh:mm:ss:zzz") << "\n\n";
            current++;
    }
    if (count > numberOfItemsToDisplay)
        stream << "... (more omitted)";

    return hoveredData.trimmed();
}
