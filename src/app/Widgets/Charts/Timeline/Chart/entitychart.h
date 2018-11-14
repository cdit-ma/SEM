#ifndef ENTITYCHART_H
#define ENTITYCHART_H

#include "../../Series/dataseries.h"
#include "../Chart/timelinechart.h"
#include "../Data/portlifecycleeventseries.h"

#include <QWidget>
#include <QPen>
#include <QBrush>

class EntityChart : public QWidget
{
    friend class TimelineChart;
    Q_OBJECT

public:
    explicit EntityChart(ViewItem* item = 0, QWidget* parent = 0);

    ViewItem* getViewItem();

    void addLifeCycleSeries(PortLifeCycleSeries* series);
    void removeLifeCycleSeries(int ID);

    void addSeries(MEDEA::DataSeries* series);
    void removeSeries(TIMELINE_SERIES_KIND seriesKind);

    QPair<double, double> getRangeX();
    QPair<double, double> getRangeY();

    const QHash<TIMELINE_SERIES_KIND, MEDEA::DataSeries *> &getSeries();
    
    QColor getSeriesColor();
    QList<QPointF> getSeriesPoints(TIMELINE_SERIES_KIND seriesKind = TIMELINE_SERIES_KIND::DATA);

    bool isHovered();

signals:
    void dataAdded(QList<QPointF> points);
    void dataHovered(QHash<TIMELINE_SERIES_KIND, QList<QPointF>> points);
    void dataRangeXChanged(double min, double max);

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

public slots:
    void seriesHovered(TIMELINE_SERIES_KIND kind);
    void setSeriesVisible(TIMELINE_SERIES_KIND kind, bool visible);
    void setHovered(bool visible);
    void setHoveredRect(QRectF rect);

private slots:
    void themeChanged();

    void rangeXChanged(double min, double max);
    void rangeYChanged(double min, double max);
    void pointsAdded(QList<QPointF> points);

    void timelineChartRangeChanged(double min, double max);

private:
    void paintSeries(QPainter& painter, TIMELINE_SERIES_KIND kind);
    void paintLifeCycleSeries(QPainter& painter);
    void paintNotificationSeries(QPainter &painter);
    void paintStateSeries(QPainter &painter);
    void paintBarSeries(QPainter &painter);
    void paintBar(QPainter &painter, const QVector<double> &data, int x, QColor color);

    void setPointWidth(double width);
    double getPointWidth(TIMELINE_SERIES_KIND kind);

    void setMin(double min);
    void setMax(double max);
    void setRange(double min, double max);
    void rangeChanged();

    void mapPointsFromRange(MEDEA::DataSeries* series = 0, QList<QPointF> points = QList<QPointF>());
    double mapValueFromRange(double value, double min, double max, double range);

    bool pointHovered(TIMELINE_SERIES_KIND kind, QPointF point, int index);
    bool pointHovered(const QRectF& hitRect);

    QHash<qint64, QRectF>& getSeriesHitRects(TIMELINE_SERIES_KIND kind);

    bool _containsYRange = false;
    bool _hovered = false;

    double _pointWidth = 12;
    double _xScale = 1;
    double _timelineRange = 0;

    double _displayedMin;
    double _displayedMax;
    double _dataMinX;
    double _dataMaxX;
    double _dataMinY;
    double _dataMaxY;

    ViewItem* _viewItem;

    QPixmap _messagePixmap;
    QPointF _cursorPoint;
    QRectF _hoveredRect;

    QColor _pointColor;
    QColor _pointBorderColor;
    QColor _backgroundColor;
    QColor _backgroundHighlightColor;
    QColor _highlightColor;
    QColor _highlightBrushColor;
    QColor _highlightTextColor;
    QColor _hoveredRectColor;
    QColor _barColor;

    QColor _defaultStateColor;
    QColor _defaultNotificationColor;
    QColor _defaultLineColor;
    QColor _stateColor;
    QColor _notificationColor;
    QColor _lineColor;

    int _borderColorDelta;
    int _colorDelta;
    int _color_s_state;
    int _color_s_notification;
    int _color_s_line;
    int _color_v_state;
    int _color_v_notification;
    int _color_v_line;

    QPen _gridPen;
    QPen _pointPen;
    QPen _pointBorderPen;
    QPen _highlightPointPen;
    QPen _hoverLinePen;
    QPen _highlightPen;

    QHash<qint64, QRectF> _dataHitRects;
    QHash<qint64, QRectF> _notificationHitRects;
    QHash<qint64, QRectF> _stateHitRects;
    QHash<qint64, QRectF> _lineHitRects;
    QHash<qint64, QRectF> _barHitRects;

    /*
    struct Series{
        MEDEA::DataSeries* series = 0;
        bool is_visible = false;
    }
    QHash<TIMELINE_SERIES_KIND, Series> series_;
    */

    PortLifeCycleSeries* _lifeCycleSeries = 0;
    QMap<LifeCycleType, QPixmap> _lifeCycleTypePixmaps;

    QHash<TIMELINE_SERIES_KIND, bool> _seriesKindVisible;
    QHash<TIMELINE_SERIES_KIND, MEDEA::DataSeries*> _seriesList;
    QHash<TIMELINE_SERIES_KIND, QList<QPointF>> _seriesPoints;
    QHash<TIMELINE_SERIES_KIND, QList<QPointF>> _mappedPoints;
    QHash<TIMELINE_SERIES_KIND, QList<QPointF>> _hoveredPoints;

    QHash<TIMELINE_SERIES_KIND, QList<qint64>> hovered_timepoints_;
    TIMELINE_SERIES_KIND _hoveredSeriesKind;

    double _barWidth;
};

#endif // ENTITYCHART_H
