#include "stateseries.h"

#include <QLineF>

/**
 * @brief MEDEA::StateSeries::StateSeries
 * @param item
 */
MEDEA::StateSeries::StateSeries(ViewItem* item)
    : DataSeries(item, TIMELINE_SERIES_KIND::STATE)
{
}


/**
 * @brief MEDEA::StateSeries::~StateSeries
 */
MEDEA::StateSeries::~StateSeries()
{
}


/**
 * @brief MEDEA::StateSeries::addLine
 * @param p1
 * @param p2
 */
void MEDEA::StateSeries::addLine(QPointF p1, QPointF p2)
{
    _lines.append(QLineF(p1, p2));
    addPoints({p1, p2});
}


/**
 * @brief MEDEA::StateSeries::getLines
 * @return
 */
const QList<QLineF>& MEDEA::StateSeries::getLines()
{
    return _lines;
}
