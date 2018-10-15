#ifndef BARSERIES_H
#define BARSERIES_H

#include "dataseries.h"

#include <QSize>
#include <QPointF>
#include <QDebug>


namespace MEDEA {

class BarData{
public:
    BarData(quint64 time, const QVector<double>& values) {
        time_ = time;
        setData(values);
    }
    void setData(const QVector<double>& values) {
        data_ = values;
    }
    quint64 getTime() {
        return time_;
    }
    const QVector<double>& getData() const {
        if (data_.isEmpty()) {
            qCritical() << "GOT EMPTY DATA FAM";
        }
        return data_;
    }
    double getMin() const {
        return data_.constLast();
    }
    double getMax() const {
        return data_.constFirst();
    }
private:
    quint64 time_;
    QVector<double> data_;
};



class BarSeries : public DataSeries
{
    Q_OBJECT

public:
    explicit BarSeries(ViewItem* item = 0);
    ~BarSeries();

    void addData(QDateTime dateTime, QVector<double> data, QSize windowSize = QSize());
    void addData(quint64 dateTimeMSecsSinceEpoch, QVector<double> data, QSize windowSize = QSize());

    QMap<quint64, QVector<double>> getData();
    
    QString getHoveredDataInformation(quint64 start_time, quint64 end_time);

    const QMap<quint64, QVector<double>>& getConstData();

    const QMap<quint64, BarData*>& getConstData2() {
        return data_map_;
    }

signals:
    void dataAdded(QList<QVector<double>> data);
    void dataAdded2();

private:
    // need to use a map to preserve the insertion order of the data
    QMap<quint64, QVector<double>> _dataMap;
    QMap<quint64, BarData*> data_map_;

};

}
#endif // BARSERIES_H
