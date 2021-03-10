#ifndef CHART_H
#define CHART_H

#include "../Chart/chartlist.h"
#include "../../Data/Series/eventseries.h"
#include "../../Data/Events/workloadevent.h"
#include "../../Data/Events/portevent.h"
#include "../../../../Controllers/ViewController/viewitem.h"
#include "../../../../theme.h"

#include "../../Data/Series/portlifecycleeventseries.h"
#include "../../Data/Series/porteventseries.h"
#include "../../Data/Series/workloadeventseries.h"
#include "../../Data/Series/markereventseries.h"
#include "../../Data/Series/cpuutilisationeventseries.h"
#include "../../Data/Series/memoryutilisationeventseries.h"
#include "../../Data/Series/networkutilisationeventseries.h"
#include "../../Data/Series/gpucomputeutilisationseries.h"
#include "../../Data/Series/gpumemoryutilisationseries.h"
#include "../../Data/Series/gputemperatureseries.h"

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
    Q_OBJECT

public:
    explicit Chart(quint32 experimentRunID, qint64 experimentStartTime, QWidget* parent = nullptr);

    quint32 getExperimentRunID() const;

    void addSeries(const QPointer<const MEDEA::EventSeries>& series);
    void removeSeries(ChartDataKind kind);

    QPointer<const EventSeries> getSeries(ChartDataKind kind) const;
    const QHash<ChartDataKind, QPointer<const EventSeries>>& getSeries() const;

    bool isHovered() const;

    QList<ChartDataKind> getHoveredSeriesKinds() const;
    QPair<qint64, qint64> getHoveredTimeRange(ChartDataKind kind) const;

    void updateVerticalMin(double min);
    void updateVerticalMax(double max);

    void updateRange(double startTime, double duration);
    void setRange(double min, double max);

    void setTimePadding(double padding) { time_padding_ = padding; } ;

    void setDisplayMinRatio(double ratio);
    void setDisplayMaxRatio(double ratio);
    void setDisplayRangeRatio(double minRatio, double maxRatio);

public slots:
    void setHovered(bool hovered);
    void setHoveredRect(QRectF rect);
    void seriesKindHovered(ChartDataKind kind);

private slots:
    void themeChanged();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    struct SeriesPaintValues {
        QColor series_color;
        double opacity = 1.0;
    };

    struct NetworkSeriesPaintValues {
        QColor sent_color;
        QColor received_color;
        QColor combined_color;
        double opacity = 1.0;
    };

    template<class T>
    struct EventSeriesPaintValues {
        QHash<T, QPixmap> pixmaps;
        SeriesPaintValues paint_val;
    };

    void outlineHoveredData(QPainter& painter);
    void displayDataMinMax(QPainter& painter);

    void paintSeries(QPainter& painter, const QPointer<const EventSeries>& series);
    void paintMarkerEventSeries(QPainter& painter, const QPointer<const MarkerEventSeries>& series);
    void paintNetworkUtilisationSeries(QPainter& painter, const QPointer<const NetworkUtilisationEventSeries>& series);

    template<class DerivedEvent>
    DerivedEvent* convertEvent(const Event* event) const { return (DerivedEvent*) event; };

    template<class DerivedEvent, class EventEnumType,
        std::enable_if_t<std::is_same<DerivedEvent, PortLifecycleEvent>::value ||
                         std::is_same<DerivedEvent, PortEvent>::value ||
                         std::is_same<DerivedEvent, WorkloadEvent>::value, int> = 1>
    void paintEventSeries(QPainter& painter, const QPointer<const EventSeries>& series, const EventSeriesPaintValues<EventEnumType>& paint_vals);

    template<class DerivedEvent,
        std::enable_if_t<std::is_same<DerivedEvent, CPUUtilisationEvent>::value ||
                         std::is_same<DerivedEvent, MemoryUtilisationEvent>::value ||
                         std::is_same<DerivedEvent, GPUComputeUtilisationEvent>::value ||
                         std::is_same<DerivedEvent, GPUMemoryUtilisationEvent>::value ||
                         std::is_same<DerivedEvent, GPUTemperatureEvent>::value, int> = 1>
    void paintSingleLineSeries(QPainter& painter, const QPointer<const EventSeries>& series, const SeriesPaintValues& paint_vals);

    QPair<QPair<int, int>, QPair<QList<Event*>::const_iterator, QList<Event*>::const_iterator>>
    getOuterDisplayIterators(const QList<Event*>& events, double target_bin_width) const;

    QVector<QList<Event*>> binEvents(const QList<Event*>& events,
                                     int bin_count,
                                     QList<Event*>::const_iterator first,
                                     QList<Event*>::const_iterator last) const;

    void drawTextRect(QPainter& painter, const QRectF& rect, const QString& text, const QColor& color, ChartDataKind series_kind);
    void drawLineFromRects(QPainter& painter, const QList<QRectF>& rects, const QColor& color, double opacity, ChartDataKind series_kind);

    bool rectHovered(ChartDataKind kind, const QRectF& hitRect);
    void clearHoveredLists();

    void updateBinnedData(ChartDataKind kind);
    void updateBinnedData(const QSet<ChartDataKind>& kinds = QSet<ChartDataKind>());

    int getBinCount(double target_bin_width) const;
    double getBinWidth(double target_bin_width) const;

    qint64 mapPixelToTime(double x);
	double mapTimeToPixel(double time);

    qint64 getDisplayMin() const;
    qint64 getDisplayMax() const;

	static QColor getContrastingColor(const QColor& color);

    void setupPaintValues(Theme& theme);
    void setupPixmaps(Theme& theme);

    quint32 experimentRunID_;
    qint64 experimentRunStartTime_;

    bool containsYRange_ = false;
    bool hovered_ = false;

    double time_padding_ = 0.01;

    double dataMinX_ = DBL_MAX;
    double dataMaxX_ = DBL_MIN;
    double dataMinY_ = DBL_MAX;
    double dataMaxY_ = DBL_MIN;
    double minRatio_ = 0.0;
    double maxRatio_ = 1.0;

    static const double background_opacity_;
    static const double border_width_;
    static const double default_bin_width_;
    static const double default_ellipse_width_;

    QRectF hoveredRect_;

    QPen defaultTextPen_;
    QPen defaultRectPen_;
    QPen defaultEllipsePen_;
    QPen highlightPen_;
    QBrush highlightBrush_;

    QColor textColor_;
    QColor hoveredRectColor_;

    QColor backgroundColor_;
    QColor backgroundDefaultColor_;
    QColor backgroundHighlightColor_;

    QHash<ChartDataKind, QPointer<const MEDEA::EventSeries>> series_pointers_;

    QHash<ChartDataKind, QPair<qint64, qint64>> hoveredSeriesTimeRange_;
    ChartDataKind hoveredSeriesKind_ = ChartDataKind::DATA;
    QList<QRectF> hoveredEllipseRects_;
    QList<QRectF> hoveredRects_;

    SeriesPaintValues marker_event_paint_vals_;
    SeriesPaintValues cpu_util_paint_vals_;
    SeriesPaintValues memory_util_paint_vals_;
    SeriesPaintValues gpu_comp_util_paint_vals_;
    SeriesPaintValues gpu_mem_util_paint_vals_;
    SeriesPaintValues gpu_temp_paint_vals_;
    NetworkSeriesPaintValues network_util_paint_vals_;
    EventSeriesPaintValues<AggServerResponse::LifecycleType> port_lifecycle_paint_vals_;
    EventSeriesPaintValues<PortEvent::PortEventType> port_event_paint_vals_;
    EventSeriesPaintValues<WorkloadEvent::WorkloadEventType> workload_event_paint_vals_;
};

}

#endif // CHART_H