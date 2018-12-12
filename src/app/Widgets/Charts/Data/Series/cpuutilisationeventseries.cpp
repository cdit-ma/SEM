#include "cpuutilisationeventseries.h"


/**
 * @brief CPUUtilisationEventSeries::CPUUtilisationEventSeries
 * @param hostname
 * @param parent
 */
CPUUtilisationEventSeries::CPUUtilisationEventSeries(QString hostname, QObject *parent)
    : MEDEA::EventSeries(parent, TIMELINE_SERIES_KIND::CPU_UTILISATION)
{
    hostname_ = hostname;
}


/**
 * @brief CPUUtilisationEventSeries::getHostname
 * @return
 */
const QString& CPUUtilisationEventSeries::getHostname() const
{
    return hostname_;
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
        QString hoveredData;
        QTextStream stream(&hoveredData);
        numberOfItemsToDisplay = qMin(count, numberOfItemsToDisplay);
        for (int i = 0; i < numberOfItemsToDisplay; i++) {
            auto event = (CPUUtilisationEvent*)(*current);
            stream << "Host: " << hostname_ << "\n"
                   << "Utilisation: " << event->getUtilisation() << "% "
                   << "at " << QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("hh:mm:ss:zzz") << "\n\n";
            current++;
        }
        if (count > 10) {
            stream << "... (more omitted)";
        }
        return hoveredData.trimmed();
    }
}
