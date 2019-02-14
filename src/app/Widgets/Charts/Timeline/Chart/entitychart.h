#ifndef ENTITYCHART_H
#define ENTITYCHART_H

#include "../Chart/timelinechart.h"
#include "../../Data/Series/eventseries.h"
#include "../../Data/Events/workloadevent.h"
#include "../../../../Controllers/ViewController/viewitem.h"

#include <QWidget>
#include <QPen>
#include <QBrush>

class EntityChart : public QWidget
{
    friend class TimelineChart;
    Q_OBJECT

public:
    explicit EntityChart(quint32 experimentRunID, QWidget* parent = 0);

    quint32 getExperimentRunID();

    void addEventSeries(MEDEA::EventSeries* series);
    void removeEventSeries(TIMELINE_DATA_KIND kind);

    bool isHovered();

    const QHash<TIMELINE_DATA_KIND, MEDEA::EventSeries*>& getSeries();
    const QList<TIMELINE_DATA_KIND> getHovereSeriesKinds();
    const QPair<qint64, qint64> getHoveredTimeRange(TIMELINE_DATA_KIND kind);

    void setRange(double min, double max);

    void setDisplayMinRatio(double ratio);
    void setDisplayMaxRatio(double ratio);
    void setDisplayRangeRatio(double minRatio, double maxRatio);

signals:
    void dataAdded(QList<QPointF> points);

public slots:
    void setHovered(bool visible);
    void setHoveredRect(QRectF rect);
    void seriesKindHovered(TIMELINE_DATA_KIND kind);
    void setSeriesKindVisible(TIMELINE_DATA_KIND kind, bool visible);

private slots:
    void themeChanged();

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    void paintSeries(QPainter& painter, TIMELINE_DATA_KIND kind);
    void paintPortLifecycleEventSeries(QPainter& painter);
    void paintWorkloadEventSeries(QPainter& painter);
    void paintCPUUtilisationEventSeries(QPainter& painter);
    void paintMemoryUtilisationEventSeries(QPainter &painter);
    void paintBarSeries(QPainter &painter);
    void paintBarData(QPainter &painter, const QRectF &barRect, const QColor &color, const QVector<double> &data);

    bool rectHovered(TIMELINE_DATA_KIND kind, const QRectF& hitRect);
    bool rectHovered(const QRectF& hitRect);

    void clearHoveredLists();
    void updateSeriesPixmaps();

    qint64 mapPixelToTime(double x);
    double mapTimeToPixel(double time);

    quint32 _experimentRunID;

    bool _containsYRange = false;
    bool _hovered = false;

    double _displayMin;
    double _displayMax;
    double _dataMinX;
    double _dataMaxX;
    double _dataMinY;
    double _dataMaxY;

    double _minRatio = 0.0;
    double _maxRatio = 1.0;

    QPixmap _messagePixmap;
    QPointF _cursorPoint;
    QRectF _hoveredRect;

    QColor _backgroundColor;
    QColor _highlightColor;
    QColor _highlightTextColor;
    QColor _hoveredRectColor;

    QColor _defaultPortLifecycleColor = Qt::gray;
    QColor _defaultWorkloadColor = Qt::gray;
    QColor _defaultUtilisationColor = Qt::lightGray;
    QColor _defaultMemoryColor = Qt::lightGray;

    QColor _portLifecycleColor = _defaultUtilisationColor;
    QColor _workloadColor = _defaultWorkloadColor;
    QColor _utilisationColor = _defaultUtilisationColor;
    QColor _memoryColor = _defaultMemoryColor;

    QPen _gridPen;
    QPen _hoverLinePen;

    /*
    struct Series{
        MEDEA::DataSeries* series = 0;
        bool is_visible = false;
    }
    QHash<TIMELINE_DATA_KIND, Series> series_;
    */

    QMap<LifecycleType, QPixmap> _lifeCycleTypePixmaps;
    QHash<WorkloadEvent::WorkloadEventType, QPixmap> _workloadEventTypePixmaps;

    QHash<TIMELINE_DATA_KIND, bool> _seriesKindVisible;
    QHash<TIMELINE_DATA_KIND, MEDEA::EventSeries*> _seriesList;

    QHash<TIMELINE_DATA_KIND, QPair<qint64, qint64>> _hoveredSeriesTimeRange;
    TIMELINE_DATA_KIND _hoveredSeriesKind;
};

#endif // ENTITYCHART_H
