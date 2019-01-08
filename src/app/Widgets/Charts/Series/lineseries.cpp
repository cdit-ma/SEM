#include "lineseries.h"

#include <QLineF>

/**
 * @brief MEDEA::LineSeries::LineSeries
 * @param item
 */
MEDEA::LineSeries::LineSeries(ViewItem* item)
    : DataSeries(item, TIMELINE_DATA_KIND::LINE)
{
}


/**
 * @brief MEDEA::LineSeries::~LineSeries
 */
MEDEA::LineSeries::~LineSeries()
{
}


/**
 * @brief MEDEA::LineSeries::addLine
 * @param p1
 * @param p2
 */
void MEDEA::LineSeries::addLine(QPointF p1, QPointF p2)
{
    addLine(QLineF(p1, p2));
}


/**
 * @brief MEDEA::LineSeries::addLine
 * @param line
 */
void MEDEA::LineSeries::addLine(QLineF line)
{
    addLines(QList<QLineF>{line});
}


/**
 * @brief MEDEA::LineSeries::addLines
 * @param lines
 */
void MEDEA::LineSeries::addLines(QList<QLineF> lines)
{
    for (QLineF line : lines) {
        DataSeries::addPoints(QList<QPointF>{line.p1(), line.p2()});
    }
    _lines.append(lines);
    emit linesAdded(lines);
}


/**
 * @brief MEDEA::LineSeries::getLines
 * @return
 */
QList<QLineF> MEDEA::LineSeries::getLines()
{
    return _lines;
}

