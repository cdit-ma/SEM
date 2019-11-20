#ifndef CHART_H
#define CHART_H

#include "../Chart/chartlist.h"
#include "../../Data/Series/eventseries.h"
#include "../../Data/Events/workloadevent.h"
#include "../../Data/Events/portevent.h"
#include "../../../../Controllers/ViewController/viewitem.h"

#include <QWidget>
#include <QPen>
#include <QBrush>

namespace MEDEA {

/**
 * @brief The Chart class renders EventSeries based on their kind, which is determined by the kind of Events stored in them.
 * This class can contain multiple EventSeries of different kinds and render them along a shared timeline (date-time) axis.
 * Each kind of series has its own render function and hence, are visualised in different ways.
 */
class Chart : public QWidget
{
    friend class ChartList;
    Q_OBJECT

public:
    explicit Chart(quint32 experimentRunID, qint64 experimentStartTime, QWidget* parent = nullptr);

    quint32 getExperimentRunID() const;

    void addSeries(EventSeries *series);
    void removeSeries(ChartDataKind kind);

    bool isHovered();

    const QHash<ChartDataKind, EventSeries*>& getSeries() const;
    const QList<ChartDataKind> getHovereSeriesKinds() const;
    const QPair<qint64, qint64> getHoveredTimeRange(ChartDataKind kind) const;

    void updateBinnedData(ChartDataKind kind);
    void updateBinnedData(QSet<ChartDataKind> kinds = QSet<ChartDataKind>());

    void updateVerticalMin(double min);
    void updateVerticalMax(double max);
    void updateRange(double startTime, double duration);

    void setRange(double min, double max);

    void setDisplayMinRatio(double ratio);
    void setDisplayMaxRatio(double ratio);
    void setDisplayRangeRatio(double minRatio, double maxRatio);

public slots:
    void setHovered(bool hovered);
    void setHoveredRect(QRectF rect);
    void seriesKindHovered(ChartDataKind kind);
    void setSeriesKindVisible(ChartDataKind kind, bool visible);

private slots:
    void themeChanged();

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

private:
    void paintSeries(QPainter& painter, const ChartDataKind kind);
    void paintPortLifecycleEventSeries(QPainter& painter);
    void paintWorkloadEventSeries(QPainter& painter);
    void paintCPUUtilisationEventSeries(QPainter& painter);
    void paintMemoryUtilisationEventSeries(QPainter &painter);
    void paintMarkerEventSeries(QPainter &painter);
    void paintPortEventSeries(QPainter &painter);

    //void paintPortLifecycleSeries(QPainter& painter);

    bool rectHovered(ChartDataKind kind, const QRectF& hitRect);
    bool rectHovered(const QRectF& hitRect);

    void clearHoveredLists();
    void updateSeriesPixmaps();

    QColor getContrastingColor(const QColor& color);

    int getBinIndexForTime(double time);
    QVector<QList<Event*>>& getBinnedData(ChartDataKind kind);

    qint64 mapPixelToTime(double x);
    double mapTimeToPixel(double time);

    quint32 experimentRunID_;
    qint64 experimentRunStartTime_;

    bool containsYRange_ = false;
    bool hovered_ = false;

    double displayMin_;
    double displayMax_;
    double dataMinX_;
    double dataMaxX_;
    double dataMinY_;
    double dataMaxY_;
    double dataHeight_ = 0;

    double minRatio_ = 0.0;
    double maxRatio_ = 1.0;

    int binCount_;
    double binPixelWidth_;
    double binTimeWidth_;

    QRectF hoveredRect_;

    QPen defaultRectPen_;
    QPen defaultEllipsePen_;
    QPen highlightPen_;
    QBrush highlightBrush_;

    QColor gridColor_;
    QColor textColor_;
    QColor highlightColor_;
    QColor highlightTextColor_;
    QColor hoveredRectColor_;

    QColor backgroundColor_;
    QColor backgroundDefaultColor_;
    QColor backgroundHighlightColor_;

    QColor defaultPortLifecycleColor_ = Qt::gray;
    QColor defaultWorkloadColor_ = Qt::gray;
    QColor defaultUtilisationColor_ = Qt::lightGray;
    QColor defaultMemoryColor_ = Qt::lightGray;
    QColor defaultMarkerColor_ = Qt::gray;
    QColor defaultPortEventColor_ = Qt::gray;

    QColor portLifecycleColor_ = defaultUtilisationColor_;
    QColor workloadColor_ = defaultWorkloadColor_;
    QColor utilisationColor_ = defaultUtilisationColor_;
    QColor memoryColor_ = defaultMemoryColor_;
    QColor markerColor_ = defaultMarkerColor_;
    QColor portEventColor_ = defaultPortEventColor_;

    double portLifecycleSeriesOpacity_ = 1.0;
    double workloadSeriesOpacity_ = 1.0;
    double cpuSeriesOpacity_ = 1.0;
    double memorySeriesOpacity_ = 1.0;
    double markerSeriesOpacity_ = 1.0;
    double portEventSeriesOpacity_ = 1.0;

    QHash<ChartDataKind, bool> seriesKindVisible_;
    QHash<ChartDataKind, EventSeries*> seriesList_;

    //QHash<ChartDataKind, QVector< QList<Event*> >> binnedData_;
    QVector<QList<Event*>> portLifecycleBinnedData_;
    QVector<QList<Event*>> workloadBinnedData_;
    QVector<QList<Event*>> cpuUtilisationBinnedData_;
    QVector<QList<Event*>> memoryUtilisationBinnedData_;
    QVector<QList<Event*>> markerBinnedData_;
    QVector<QList<Event*>> portEventBinnedData_;
    QVector<QList<Event*>> emptyBinnedData_;

    QHash<ChartDataKind, QPair<qint64, qint64>> hoveredSeriesTimeRange_;
    ChartDataKind hoveredSeriesKind_;
    QList<QRectF> hoveredEllipseRects_;
    QList<QRectF> hoveredRects_;

    QHash<AggServerResponse::LifecycleType, QPixmap> lifeCycleTypePixmaps_;
    QHash<WorkloadEvent::WorkloadEventType, QPixmap> workloadEventTypePixmaps_;
    QHash<PortEvent::PortEventType, QPixmap> portEventTypePixmaps_;

    /*
    struct Series{
        DataSeries* series = 0;
        bool is_visible = false;
    }
    QHash<ChartDataKind, Series> series_;
    */
};

}

#endif // CHART_H
