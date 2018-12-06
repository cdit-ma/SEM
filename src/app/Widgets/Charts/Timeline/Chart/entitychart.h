#ifndef ENTITYCHART_H
#define ENTITYCHART_H

#include "../Chart/timelinechart.h"
#include "../../Series/dataseries.h"
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
    int getViewItemID();

    void addEventSeries(MEDEA::EventSeries* series);
    void removeEventSeries(QString ID);

    void addSeries(MEDEA::DataSeries* series);
    void removeSeries(TIMELINE_SERIES_KIND seriesKind);

    const QHash<TIMELINE_SERIES_KIND, MEDEA::DataSeries*>& getSeries();
    const QList<TIMELINE_SERIES_KIND> getHovereSeriesKinds();
    const QPair<qint64, qint64> getHoveredTimeRange(TIMELINE_SERIES_KIND kind);

    QPair<double, double> getRangeX();
    QPair<double, double> getRangeY();
    
    QColor getSeriesColor();
    QList<QPointF> getSeriesPoints(TIMELINE_SERIES_KIND seriesKind = TIMELINE_SERIES_KIND::DATA);

    bool isHovered();

signals:
    void dataAdded(QList<QPointF> points);
    void dataRangeXChanged(double min, double max);

public slots:
    void setHovered(bool visible);
    void setHoveredRect(QRectF rect);
    void seriesKindHovered(TIMELINE_SERIES_KIND kind);
    void setSeriesKindVisible(TIMELINE_SERIES_KIND kind, bool visible);

private slots:
    void themeChanged();

    void rangeXChanged(double min, double max);
    void rangeYChanged(double min, double max);
    void pointsAdded(QList<QPointF> points);

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    void paintEventSeries(QPainter& painter);
    void paintSeries(QPainter& painter, TIMELINE_SERIES_KIND kind);
    void paintNotificationSeries(QPainter &painter);
    void paintStateSeries(QPainter &painter);
    void paintBarSeries(QPainter &painter);
    void paintBarData(QPainter &painter, const QRectF &barRect, const QColor &color, const QVector<double> &data);

    bool rectHovered(TIMELINE_SERIES_KIND kind, const QRectF& hitRect);
    bool rectHovered(const QRectF& hitRect);

    void clearHoveredLists();

    void setPointWidth(double width);
    double getPointWidth(TIMELINE_SERIES_KIND kind);

    void setMin(double min);
    void setMax(double max);
    void setRange(double min, double max);

    qint64 mapPixelToTime(double x);
    double mapTimeToPixel(double time);

    bool _containsYRange = false;
    bool _hovered = false;

    double _pointWidth = 12;

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

    /*
    struct Series{
        MEDEA::DataSeries* series = 0;
        bool is_visible = false;
    }
    QHash<TIMELINE_SERIES_KIND, Series> series_;
    */

    QHash<TIMELINE_SERIES_KIND, bool> _seriesKindVisible;
    QHash<TIMELINE_SERIES_KIND, MEDEA::DataSeries*> _seriesList;
    QHash<TIMELINE_SERIES_KIND, QList<QPointF>> _seriesPoints;
    QHash<TIMELINE_SERIES_KIND, QList<QPointF>> _mappedPoints;

    QHash<TIMELINE_SERIES_KIND, QPair<qint64, qint64>> _hoveredSeriesTimeRange;
    TIMELINE_SERIES_KIND _hoveredSeriesKind;

    MEDEA::EventSeries* _eventSeries = 0;

};

#endif // ENTITYCHART_H
