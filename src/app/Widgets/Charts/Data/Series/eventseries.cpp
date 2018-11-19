#include "eventseries.h"

#include <QDateTime>


/**
 * @brief MEDEA::EventSeries::BaseSeries
 * @param parent
 */
MEDEA::EventSeries::EventSeries(QObject* parent)
    : QObject(parent)
{

}


/**
 * @brief MEDEA::EventSeries::addEvent
 * @param event
 */
void MEDEA::EventSeries::addEvent(MEDEA::Event* event)
{
    events_.append(event);
}


/**
 * @brief MEDEA::EventSeries::getEvents
 * @return
 */
const QList<MEDEA::Event*>& MEDEA::EventSeries::getEvents()
{
    return events_;
}


/**
 * @brief MEDEA::EventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @return
 */
QString MEDEA::EventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS)
{
    return "Hovered range: " +
            QDateTime::fromMSecsSinceEpoch(fromTimeMS).toString("hh:mm:ss:zz") +
            ", " +
            QDateTime::fromMSecsSinceEpoch(toTimeMS).toString("hh:mm:ss:zz");
}
