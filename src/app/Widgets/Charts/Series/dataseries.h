#ifndef DATASERIES_H
#define DATASERIES_H

#include <QObject>
#include <QDateTime>
#include <QPointF>
#include "../../../Controllers/ViewController/viewitem.h"

enum class TIMELINE_SERIES_KIND{DATA, STATE, NOTIFICATION, LINE, BAR};

namespace MEDEA {

class DataSeries : public QObject
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

    virtual QString getHoveredDataInformation(quint64 start_time, quint64 end_time){
        return QString();
    }

    int getID();
    QString getLabel();
    ViewItem* getViewItem();
    TIMELINE_SERIES_KIND getSeriesKind();

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
    TIMELINE_SERIES_KIND _seriesKind;
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
