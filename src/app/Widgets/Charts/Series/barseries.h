#ifndef BARSERIES_H
#define BARSERIES_H

#include "dataseries.h"

#include <QSize>
#include <QPointF>
#include <QDebug>


namespace MEDEA {

class BarData{
public:
    BarData(qint64 time, const QVector<double>& values) {
        time_ = time;
        setData(values);
    }
    void setData(const QVector<double>& values) {
        data_ = values;
    }
    qint64 getTime() {
        return time_;
    }
    const QVector<double>& getData() const {
        if (data_.isEmpty()) {
            qCritical() << "GOT EMPTY DATA FAM";
        }
        return data_;
    }
    double getMin() const {
        if (data_.count() == 1)
            return 0.0;
        return data_.constLast();
    }
    double getMax() const {
        return data_.constFirst();
    }
private:
    qint64 time_;
    QVector<double> data_;
};



class BarSeries : public DataSeries
{
    Q_OBJECT

public:
    explicit BarSeries(ViewItem* item = 0);
    ~BarSeries();

    void addData(QDateTime dateTime, QVector<double> data, QSize windowSize = QSize());
    void addData(qint64 dateTimeMSecsSinceEpoch, QVector<double> data, QSize windowSize = QSize());

    QMap<qint64, QVector<double>> getData();

    QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS, int numberOfItemsToDisplay = getDefaultNumberOfItemsToDisplay(), QString displayFormat = getDefaultDisplayFormat());

    const QMap<qint64, QVector<double>>& getConstData();

    const QMap<qint64, BarData*>& getConstData2() {
        return data_map_;
    }

signals:
    void dataAdded(QList<QVector<double>> data);
    void dataAdded2();

private:
    // need to use a map to preserve the insertion order of the data
    QMap<qint64, QVector<double>> _dataMap;
    QMap<qint64, BarData*> data_map_;
};

}
#endif // BARSERIES_H
