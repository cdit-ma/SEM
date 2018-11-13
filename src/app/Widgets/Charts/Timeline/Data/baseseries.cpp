#include "baseseries.h"

#include <QDateTime>


/**
 * @brief BaseSeries::BaseSeries
 * @param parent
 */
BaseSeries::BaseSeries(QObject* parent)
    : QObject(parent)
{

}


/**
 * @brief BaseSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @return
 */
QString BaseSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS)
{
    return "Hovered range: " +
            QDateTime::fromMSecsSinceEpoch(fromTimeMS).toString("hh:mm:ss:zz") +
            ", " +
            QDateTime::fromMSecsSinceEpoch(toTimeMS).toString("hh:mm:ss:zz");
}
