#ifndef DATASERIES_H
#define DATASERIES_H

#include <QDateTime>
#include <QPointF>

#include "../../../Controllers/ViewController/viewitem.h"
#include "../Data/Series/eventseries.h"

namespace MEDEA {

class DataSeries : public EventSeries
{
    Q_OBJECT

protected:
    //explicit DataSeries(ViewItem* item = 0, TIMELINE_SERIES_KIND kind = TIMELINE_SERIES_KIND::DATA);
    ~DataSeries();

    void addPoint(QPointF point);
    //void addPoints(QList<QPointF> points);

public:
    explicit DataSeries(ViewItem* item = 0, TIMELINE_SERIES_KIND kind = TIMELINE_SERIES_KIND::DATA);
    void addPoints(QList<QPointF> points);

    /*
    virtual QString getHoveredDataInformation(qint64 start_time, qint64 end_time){
        return QString();
    }
    */

    //int getID();
    QString getLabel();
    ViewItem* getViewItem();

    bool isStateSeries();
    bool isNotificationSeries();
    bool isLineSeries();
    bool isBarSeries();

    const QList<QPointF>& getConstPoints();
    QPair<double, double> getRangeX();
    QPair<double, double> getRangeY();

signals:
    void pointsAdded(QList<QPointF> points);

    void minXChanged(double x);
    void maxXChanged(double x);
    void minYChanged(double y);
    void maxYChanged(double y);

    void rangeXChanged(double min, double max);
    void rangeYChanged(double min, double max);

private:
    ViewItem* _viewItem;
    int _ID;

    QList<QPointF> _points;
    double _minX;
    double _maxX;
    double _minY;
    double _maxY;

    static int series_ID;
};

}

#endif // DATASERIES_H
