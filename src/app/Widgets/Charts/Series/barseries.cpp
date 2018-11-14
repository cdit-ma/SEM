#include "barseries.h"

#include <QDebug>

/**
 * @brief MEDEA::BarSeries::BarSeries
 * @param item
 */
MEDEA::BarSeries::BarSeries(ViewItem *item)
    : DataSeries(item, TIMELINE_SERIES_KIND::BAR)
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
 * @return
 */
QString MEDEA::BarSeries::getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS)
{
    const auto& data = getConstData2();
    auto current = data.lowerBound(fromTimeMS);
    auto upper = data.upperBound(toTimeMS);

    //qDebug() << "from: " << QDateTime::fromMSecsSinceEpoch(fromTimeMS).toString("MMMM d, hh:mm:ss:zzz");
    //qDebug() << "to: " << QDateTime::fromMSecsSinceEpoch(toTimeMS).toString("MMMM d, hh:mm:ss:zzz");

    int count = std::distance(current, upper);
    if (count > 1) {
        return QString::number(count);
    }

    QString text;
    QTextStream stream(&text);

    for (;current != upper; current++) {
        //const auto& barData = current.value();
        //stream << barData->getMax() << " "; //\n";
        //stream << current.key() << "\n";
        stream << QDateTime::fromMSecsSinceEpoch(current.key()).toString("MMMM d, hh:mm:ss:zzz") << "\n";
    }

    return text.trimmed();
}


/**
 * @brief MEDEA::BarSeries::getConstData
 * @return
 */
const QMap<qint64, QVector<double>>& MEDEA::BarSeries::getConstData()
{
    return _dataMap;
}

