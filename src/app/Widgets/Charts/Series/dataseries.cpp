#include "dataseries.h"

#include <algorithm>
#include <QDebug>
#include <math.h>
#include <float.h>


int MEDEA::DataSeries::series_ID = 0;

/**
 * @brief MEDEA::DataSeries::DataSeries
 * @param item
 * @param kind
 */
MEDEA::DataSeries::DataSeries(ViewItem* item, TIMELINE_SERIES_KIND kind)
    : EventSeries(item)
{
    _viewItem = item;
    _ID = series_ID++;
    _seriesKind = kind;

    _minX = DBL_MAX;
    _maxX = DBL_MIN;
    _minY = DBL_MAX;
    _maxY = DBL_MIN;
}


/**
 * @brief MEDEA::DataSeries::~DataSeries
 */
MEDEA::DataSeries::~DataSeries()
{

}


/**
 * @brief MEDEA::DataSeries::addPoint
 * @param point
 */
void MEDEA::DataSeries::addPoint(QPointF point)
{
    addPoints(QList<QPointF>{point});
}


/**
 * @brief MEDEA::DataSeries::addPoints
 * @param points
 */
void MEDEA::DataSeries::addPoints(QList<QPointF> points)
{
    if (points.isEmpty())
        return;

    _points.append(points);
    emit pointsAdded(points);
    //return;

    /*
    // these return the iterators to the points with min/max x and min/max y
    auto x_minmax = std::minmax_element(points.begin(), points.end(), [](QPointF const& a, QPointF const& b){
       return a.x() < b.x();
    });
    auto y_minmax = std::minmax_element(points.begin(), points.end(), [](QPointF const& a, QPointF const& b){
       return a.y() < b.y();
    });

    double newMinX = (*x_minmax.first).x();
    double newMaxX = (*x_minmax.second).x();
    double newMinY = (*y_minmax.first).y();
    double newMaxY = (*y_minmax.second).y();
    bool xChanged = false;
    bool yChanged = false;

    //qCritical() << *x_minmax.first << " " << *x_minmax.second;
    //qCritical() << *y_minmax.first << " " << *y_minmax.second;
    */

    double newMinX = _minX;
    double newMaxX = _maxX;
    double newMinY = _minY;
    double newMaxY = _maxY;
    bool xChanged = false;
    bool yChanged = false;

    // update the stored range
    for (QPointF p : points) {
        newMinX = qMin(p.x(), newMinX);
        newMaxX = qMax(p.x(), newMaxX);
        newMinY = qMin(p.y(), newMinY);
        newMaxY = qMax(p.y(), newMaxY);
    }

    if (newMinX != _minX) {
        _minX = newMinX;
        xChanged = true;
        emit minXChanged(_minX);
    }

    if (newMaxX != _maxX) {
        _maxX = newMaxX;
        xChanged = true;
        emit maxXChanged(_maxX);
    }

    if (newMinY != _minY) {
        _minY = newMinY;
        yChanged = true;
        emit minYChanged(_minY);
    }

    if (newMaxY != _maxY) {
        _maxY = newMaxY;
        yChanged = true;
        emit maxYChanged(_maxY);
    }

    if (xChanged)
        emit rangeXChanged(_minX, _maxX);
    if (yChanged)
        emit rangeYChanged(_minY, _maxY);
}

/*QString MEDEA::DataSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS)
{
    return "MEDEA::DataSeries::getHoveredDataString()";
}*/


/**
 * @brief MEDEA::DataSeries::getID
 * @return
 */
int MEDEA::DataSeries::getID()
{
    return _ID;
}


/**
 * @brief MEDEA::DataSeries::getLabel
 * @return
 */
QString MEDEA::DataSeries::getLabel()
{
    if (_viewItem) {
        return _viewItem->getData("label").toString();
    }
    return "";
}


/**
 * @brief MEDEA::DataSeries::getViewItem
 * @return
 */
ViewItem* MEDEA::DataSeries::getViewItem()
{
    return _viewItem;
}


/**
 * @brief MEDEA::DataSeries::getSeriesKind
 * @return
 */
TIMELINE_SERIES_KIND MEDEA::DataSeries::getSeriesKind()
{
    return _seriesKind;
}


/**
 * @brief MEDEA::DataSeries::isStateSeries
 * @return
 */
bool MEDEA::DataSeries::isStateSeries()
{
    return _seriesKind == TIMELINE_SERIES_KIND::STATE;
}


/**
 * @brief MEDEA::DataSeries::isNotificationSeries
 * @return
 */
bool MEDEA::DataSeries::isNotificationSeries()
{
    return _seriesKind == TIMELINE_SERIES_KIND::NOTIFICATION;
}


/**
 * @brief MEDEA::DataSeries::isLineSeries
 * @return
 */
bool MEDEA::DataSeries::isLineSeries()
{
    return _seriesKind == TIMELINE_SERIES_KIND::LINE;
}


/**
 * @brief MEDEA::DataSeries::isBarSeries
 * @return
 */
bool MEDEA::DataSeries::isBarSeries()
{
    return _seriesKind == TIMELINE_SERIES_KIND::BAR;
}


/**
 * @brief MEDEA::DataSeries::getConstPoints
 * @return
 */
const QList<QPointF> &MEDEA::DataSeries::getConstPoints()
{
    return _points;
}


/**
 * @brief MEDEA::DataSeries::getRangeX
 * @return
 */
QPair<double, double> MEDEA::DataSeries::getRangeX()
{
    return QPair<double, double>{_minX, _maxX};
}


/**
 * @brief MEDEA::DataSeries::getRangeY
 * @return
 */
QPair<double, double> MEDEA::DataSeries::getRangeY()
{
    return QPair<double, double>{_minY, _maxY};
}
