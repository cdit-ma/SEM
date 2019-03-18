#include "markereventseries.h"
#include "../Events/markerevent.h"

/**
 * @brief MarkerEventSeries::MarkerEventSeries
 * @param ID
 * @param parent
 */
MarkerEventSeries::MarkerEventSeries(QString ID, QObject *parent)
    : MEDEA::EventSeries(ID, TIMELINE_DATA_KIND::MARKER, parent) {}


/**
 * @brief MarkerEventSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString MarkerEventSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat)
{
    return "";
}
