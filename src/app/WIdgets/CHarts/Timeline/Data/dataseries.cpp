#include "dataseries.h"

#include <QDateTime>

/**
 * @brief MEDEA::DataSeries::DataSeries
 * @param parent
 */
MEDEA::DataSeries::DataSeries(QObject* parent)
    : QObject(parent)
{

}


/**
 * @brief MEDEA::DataSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @return
 */
QString MEDEA::DataSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS)
{
    return "Hovered range: " +
            QDateTime::fromMSecsSinceEpoch(fromTimeMS).toString("hh:mm:ss:zz") +
            ", " +
            QDateTime::fromMSecsSinceEpoch(toTimeMS).toString("hh:mm:ss:zz");
}
