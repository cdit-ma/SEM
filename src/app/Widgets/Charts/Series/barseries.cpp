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
 * @brief MEDEA::BarSeries::getHoveredDataInformation
 * @param start_time
 * @param end_time
 * @return 
 */
QString MEDEA::BarSeries::getHoveredDataInformation(quint64 start_time, quint64 end_time)
{
    const auto& data = getConstData();
    auto current = data.lowerBound(start_time);
    auto upper = data.upperBound(end_time);

    QString text;
    QTextStream stream(&text);

    for (;current != upper; current++) {
        const auto& current_x = current.key();
        const auto& y_array = current.value();

        QDateTime dt; dt.setMSecsSinceEpoch(current_x);
        stream << dt.toString("MMMM d, hh:mm:ss:zzz") << "\n";

        for (const auto& val : y_array) {
            stream << val << " ";
        }
        stream << "\n";
    }
    
    return text;
}


/**
 * @brief MEDEA::BarSeries::addData
 * @param dateTimeMSecsSinceEpoch
 * @param data
 * @param windowSize
 */
void MEDEA::BarSeries::addData(quint64 dateTimeMSecsSinceEpoch, QVector<double> data, QSize windowSize)
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
QMap<quint64, QVector<double> > MEDEA::BarSeries::getData()
{
    return _dataMap;
}


/**
 * @brief MEDEA::BarSeries::getConstData
 * @return
 */
const QMap<quint64, QVector<double>>& MEDEA::BarSeries::getConstData(){
    return _dataMap;
}

