#include "barseries.h"

#include <QDebug>

/**
 * @brief MEDEA::BarSeries::BarSeries
 * @param item
 */
MEDEA::BarSeries::BarSeries(ViewItem *item)
    : DataSeries(item, TIMELINE_DATA_KIND::BAR)
{

}


/**
 * @brief MEDEA::BarSeries::~BarSeries
 */
MEDEA::BarSeries::~BarSeries()
{
}


/**
 * @brief MEDEA::BarSeries::addData
 * @param timeStamp
 * @param data
 * @param windowSize
 */
void MEDEA::BarSeries::addData(QDateTime timeStamp, QVector<double> data, QSize windowSize)
{
    addData(timeStamp.toMSecsSinceEpoch(), data, windowSize);
}


/**
 * @brief MEDEA::BarSeries::addData
 * @param dateTimeMSecsSinceEpoch
 * @param data
 * @param windowSize
 */
void MEDEA::BarSeries::addData(qint64 dateTimeMSecsSinceEpoch, QVector<double> data, QSize windowSize)
{
    if (!data.isEmpty()) {
        auto data_value = new MEDEA::BarData(dateTimeMSecsSinceEpoch, data);
        data_map_.insert(data_value->getTime(), data_value);

        for (double d : data) {
            DataSeries::addPoint(QPointF(dateTimeMSecsSinceEpoch, d));
        }

        /*()
        for (double d : data) {
            DataSeries::addPoint(QPointF(dateTimeMSecsSinceEpoch, d));
        }
        _dataMap.insert(dateTimeMSecsSinceEpoch, data);*/
        //emit dataAdded({data});
        emit dataAdded2();
    }
}


/**
 * @brief MEDEA::BarSeries::getData
 * @return
 */
QMap<qint64, QVector<double> > MEDEA::BarSeries::getData()
{
    return _dataMap;
}


/**
 * @brief MEDEA::BarSeries::getHoveredDataString
 * @param fromTimeMS
 * @param toTimeMS
 * @param numberOfItemsToDisplay
 * @param displayFormat
 * @return
 */
QString MEDEA::BarSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay, QString displayFormat)
{
    const auto& data = getConstData2();
    auto current = data.lowerBound(fromTimeMS);
    auto upper = data.upperBound(toTimeMS);

    int count = std::distance(current, upper);
    if (count <= 0)
        return "";

    QString hovereData = "";
    QTextStream stream(&hovereData);
    numberOfItemsToDisplay = qMin(count, numberOfItemsToDisplay);

    for (int i = 0; i < numberOfItemsToDisplay; i++) {
        stream << QDateTime::fromMSecsSinceEpoch(current.key()).toString(displayFormat) << "\n";
        current++;
    }

    if (count > numberOfItemsToDisplay)
        stream << "... (more omitted)";

    return hovereData.trimmed();
}


/**
 * @brief MEDEA::BarSeries::getConstData
 * @return
 */
const QMap<qint64, QVector<double>>& MEDEA::BarSeries::getConstData()
{
    return _dataMap;
}

