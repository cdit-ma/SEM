#include "chart.h"
#include "../../../../theme.h"
#include "../../Data/Events/portlifecycleevent.h"
#include "../../Data/Events/cpuutilisationevent.h"
#include "../../Data/Events/memoryutilisationevent.h"
#include "../../Data/Series/markereventseries.h"
#include "../../Data/Series/networkutilisationeventseries.h"

#include <QPainter>

#include <algorithm>
#include <iostream>

#define BACKGROUND_OPACITY 0.25
#define BORDER_WIDTH 2.0

#define PEN_WIDTH 1.0
#define BIN_WIDTH 22.0
#define POINT_WIDTH 8.0

using namespace MEDEA;

/**
 * @brief Chart::Chart
 * @param experimentRunID
 * @param experimentStartTime
 * @param parent
 */
Chart::Chart(quint32 experimentRunID, qint64 experimentStartTime, QWidget* parent)
    : QWidget(parent),
      experimentRunID_(experimentRunID),
      experimentRunStartTime_(experimentStartTime)
{
	setMouseTracking(true);

    connect(Theme::theme(), &Theme::theme_Changed, this, &Chart::themeChanged);
    themeChanged();
}

/**
 * @brief Chart::getExperimentRunID
 * @return
 */
quint32 Chart::getExperimentRunID() const
{
	return experimentRunID_;
}

/**
 * @brief Chart::addSeries
 * @param series
 */
void Chart::addSeries(const QPointer<const MEDEA::EventSeries>& series)
{
    if (series.isNull()) {
        return;
    }

    if (series->getKind() == ChartDataKind::CPU_UTILISATION ||
        series->getKind() == ChartDataKind::MEMORY_UTILISATION ||
        series->getKind() == ChartDataKind::NETWORK_UTILISATION) {

        containsYRange_ = true;
        connect(series, &EventSeries::minYValueChanged, this, &Chart::updateVerticalMin);
        connect(series, &EventSeries::maxYValueChanged, this, &Chart::updateVerticalMax);

        const auto& cpu_util = dynamic_cast<const CPUUtilisationEventSeries*>(series.data());
        const auto& mem_util = dynamic_cast<const MemoryUtilisationEventSeries*>(series.data());
        const auto& net_util = dynamic_cast<const NetworkUtilisationEventSeries*>(series.data());

        if (cpu_util) {
            dataMinY_ = cpu_util->getMinUtilisation();
            dataMaxY_ = cpu_util->getMaxUtilisation();
        } else if (mem_util) {
            dataMinY_ = mem_util->getMinUtilisation();
            dataMaxY_ = mem_util->getMaxUtilisation();
        } else if (net_util) {
            dataMinY_ = net_util->getMinUtilisation();
            dataMaxY_ = net_util->getMaxUtilisation();
        }
    }

    series_pointers_[series->getKind()] = series;
    update();
}

/**
 * @brief Chart::removeSeries
 * @param kind
 */
void Chart::removeSeries(ChartDataKind kind)
{
    series_pointers_.remove(kind);
    update();
}

/**
 * @brief Chart::getSeries
 * @param kind
 * @return
 */
QPointer<const EventSeries> Chart::getSeries(ChartDataKind kind) const
{
    return series_pointers_.value(kind, {});
}

/**
 * @brief Chart::getSeries
 * @return
 */
const QHash<ChartDataKind, QPointer<const EventSeries>>& Chart::getSeries() const
{
    return series_pointers_;
}

/**
 * @brief Chart::getHoveredSeriesKinds
 * @return
 */
QList<ChartDataKind> Chart::getHoveredSeriesKinds() const
{
	return hoveredSeriesTimeRange_.keys();
}

/**
 * @brief Chart::getHoveredTimeRange
 * @param kind
 * @return
 */
QPair<qint64, qint64> Chart::getHoveredTimeRange(ChartDataKind kind) const
{
	return hoveredSeriesTimeRange_.value(kind, {-1, -1});
}

/**
 * @brief Chart::setRange
 * @param min
 * @param max
 */
void Chart::setRange(double min, double max)
{
	// add 1% on either side to include border values
	auto range = max - min;
	min = min - (range * 0.01);
	max = max + (range * 0.01);

	dataMinX_ = min;
	dataMaxX_ = max;

	//displayMin_ = (dataMaxX_ - dataMinX_) * minRatio_ + dataMinX_;
	//displayMax_ = (dataMaxX_ - dataMinX_) * maxRatio_ + dataMinX_;

	update();
}

/**
 * @brief Chart::setDisplayMinRatio
 * @param ratio
 */
void Chart::setDisplayMinRatio(double ratio)
{
	minRatio_ = ratio;
	//displayMin_ = (dataMaxX_ - dataMinX_) * ratio + dataMinX_;
	update();
}

/**
 * @brief Chart::setDisplayMaxRatio
 * @param ratio
 */
void Chart::setDisplayMaxRatio(double ratio)
{
	maxRatio_ = ratio;
	//displayMax_ = (dataMaxX_ - dataMinX_) * ratio + dataMinX_;
	update();
}

/**
 * @brief Chart::setDisplayRangeRatio
 * @param minRatio
 * @param maxRatio
 */
void Chart::setDisplayRangeRatio(double minRatio, double maxRatio)
{
	minRatio_ = minRatio;
	maxRatio_ = maxRatio;
	//displayMin_ = (dataMaxX_ - dataMinX_) * minRatio + dataMinX_;
	//displayMax_ = (dataMaxX_ - dataMinX_) * maxRatio + dataMinX_;
	update();
}

/**
 * @brief Chart::updateRange
 * @param startTime
 * @param duration
 */
void Chart::updateRange(double startTime, double duration)
{
	if (startTime == 0.0) {
		startTime = experimentRunStartTime_;
	}
	setRange(startTime, startTime + duration);
	setDisplayRangeRatio(0.0, 1.0);
	updateBinnedData();
}

/**
 * @brief Chart::updateBinnedData
 * @param kinds
 */
void Chart::updateBinnedData(const QSet<ChartDataKind>& kinds)
{
	// TODO: Instead of re-calculating the binned data in the paint method, calculate and store the binned data per series
	//  and only recalculate it when the data or diaplay range has changed, or when the widget is resized.
	//  This should do a lot less calculations and provide smoother rendering of the charts
    /*
    if (kinds.isEmpty()) {
        kinds = seriesList_.keys().toSet();
    }
    for (auto kind : kinds) {
        updateBinnedData(kind);
    }
    update();
    */
}

/**
 * @brief Chart::updateVerticalMin
 * @param min
 */
void Chart::updateVerticalMin(double min)
{
	dataMinY_ = min;
	update();
}

/**
 * @brief Chart::updateChartHeight
 * @param max
 */
void Chart::updateVerticalMax(double max)
{
	dataMaxY_ = max;
	update();
}

/**
 * @brief Chart::isHovered
 * @return
 */
bool Chart::isHovered() const
{
	return hovered_;
}

/**
 * @brief Chart::setHovered
 * @param hovered
 */
void Chart::setHovered(bool hovered)
{
	hovered_ = hovered;
	if (hovered) {
		backgroundColor_ = backgroundHighlightColor_;
	} else {
		backgroundColor_ = backgroundDefaultColor_;
	}
	update();
}

/**
 * @brief Chart::setHoveredRect
 * @param rect
 */
void Chart::setHoveredRect(QRectF rect)
{
	if (rect != hoveredRect_) {
		QPoint pos = mapFromParent(rect.topLeft().toPoint());
		rect.moveTo(pos.x(), 0);
		hoveredRect_ = rect;
	}
}

/**
 * @brief Chart::seriesKindHovered
 * @param kind
 */
void Chart::seriesKindHovered(ChartDataKind kind)
{
    if (kind == hoveredSeriesKind_) {
        return;
    }

    portLifecycleColor_ = backgroundColor_;
    workloadColor_ = backgroundColor_;
    utilisationColor_ = backgroundColor_;
    memoryColor_ = backgroundColor_;
    markerColor_ = backgroundColor_;
    portEventColor_ = backgroundColor_;
    networkColor_sent_ = backgroundColor_;
    networkColor_received_ = backgroundColor_;
    networkColor_combined_ = backgroundColor_;

    double alpha = 0.25;
    portLifecycleSeriesOpacity_ = alpha;
    workloadSeriesOpacity_ = alpha;
    cpuSeriesOpacity_ = alpha;
    memorySeriesOpacity_ = alpha;
    markerSeriesOpacity_ = alpha;
    portEventSeriesOpacity_ = alpha;
    networkSeriesOpacity_ = alpha;

    switch (kind) {
        case ChartDataKind::PORT_LIFECYCLE: {
            portLifecycleColor_ = defaultPortLifecycleColor_;
            portLifecycleSeriesOpacity_ = 1.0;
            break;
        }
        case ChartDataKind::WORKLOAD: {
            workloadColor_ = defaultWorkloadColor_;
            workloadSeriesOpacity_ = 1.0;
            break;
        }
        case ChartDataKind::CPU_UTILISATION: {
            utilisationColor_ = defaultUtilisationColor_;
            cpuSeriesOpacity_ = 1.0;
            break;
        }
        case ChartDataKind::MEMORY_UTILISATION: {
            memoryColor_ = defaultMemoryColor_;
            memorySeriesOpacity_ = 1.0;
            break;
        }
        case ChartDataKind::MARKER: {
            markerColor_ = defaultMarkerColor_;
            markerSeriesOpacity_ = 1.0;
            break;
        }
        case ChartDataKind::PORT_EVENT: {
            portEventColor_ = defaultPortEventColor_;
            portEventSeriesOpacity_ = 1.0;
            break;
        }
        case ChartDataKind::NETWORK_UTILISATION: {
            networkColor_sent_ = defaultNetworkColor_sent_;
            networkColor_received_ = defaultNetworkColor_received_;
            networkColor_combined_ = Qt::blue;
            networkSeriesOpacity_ = 1.0;
            break;
        }
        default: {
            // clear hovered state
            portLifecycleColor_ = defaultPortLifecycleColor_;
            workloadColor_ = defaultWorkloadColor_;
            utilisationColor_ = defaultUtilisationColor_;
            memoryColor_ = defaultMemoryColor_;
            markerColor_ = defaultMarkerColor_;
            portEventColor_ = defaultPortEventColor_;
            networkColor_sent_ = defaultNetworkColor_sent_;
            networkColor_received_ = defaultNetworkColor_received_;
            networkColor_combined_ = Qt::blue;
            portLifecycleSeriesOpacity_ = 1.0;
            workloadSeriesOpacity_ = 1.0;
            cpuSeriesOpacity_ = 1.0;
            memorySeriesOpacity_ = 1.0;
            markerSeriesOpacity_ = 1.0;
            portEventSeriesOpacity_ = 1.0;
            networkSeriesOpacity_ = 1.0;
            break;
        }
    }

	hoveredSeriesKind_ = kind;
	updateSeriesPixmaps();
	update();
}

/**
 * @brief Chart::themeChanged
 */
void Chart::themeChanged()
{
	Theme* theme = Theme::theme();
	setFont(theme->getSmallFont());

	defaultPortLifecycleColor_ = QColor(235,123,255);
	portLifecycleColor_ = defaultPortLifecycleColor_;

	defaultWorkloadColor_ = QColor(0,206,209);
	workloadColor_ = defaultWorkloadColor_;

	defaultUtilisationColor_ = QColor(30,144,255);
	utilisationColor_ = defaultUtilisationColor_;

	defaultMemoryColor_ = theme->getSeverityColor(Notification::Severity::SUCCESS);
	memoryColor_ = defaultMemoryColor_;

	defaultMarkerColor_ = QColor(221,188,153);
	markerColor_ = defaultMarkerColor_;

	defaultPortEventColor_ = theme->getSeverityColor(Notification::Severity::WARNING);
	portEventColor_ = defaultPortEventColor_;

    if (theme->getTextColor() == theme->black()) {
        defaultNetworkColor_sent_ = Qt::darkCyan;
        defaultNetworkColor_received_ = Qt::darkMagenta;
    } else {
        defaultNetworkColor_sent_ = Qt::cyan;
        defaultNetworkColor_received_ = Qt::magenta;
    }
    networkColor_sent_ = defaultNetworkColor_sent_;
    networkColor_received_ = defaultNetworkColor_received_;

    backgroundDefaultColor_ = theme->getAltBackgroundColor();
    backgroundDefaultColor_.setAlphaF(BACKGROUND_OPACITY);
    backgroundHighlightColor_ = theme->getActiveWidgetBorderColor();
    backgroundHighlightColor_.setAlphaF(BACKGROUND_OPACITY * 2.0);

    textColor_ = theme->getTextColor();
    gridColor_ = theme->getAltTextColor();
    backgroundColor_ = backgroundDefaultColor_;
    highlightColor_ = theme->getHighlightColor();
    hoveredRectColor_ = theme->getActiveWidgetBorderColor();

    defaultTextPen_ = QPen(textColor_, 2.0);
	defaultRectPen_ = QPen(gridColor_, 0.5);
	defaultEllipsePen_ = QPen(gridColor_, 2.0);
	highlightPen_ = QPen(highlightColor_, 2.0);
	highlightBrush_ = QBrush(getContrastingColor(textColor_));

	updateSeriesPixmaps();
}

/**
 * @brief Chart::resizeEvent
 * @param event
 */
void Chart::resizeEvent(QResizeEvent* event)
{
	updateBinnedData();
	update();
	QWidget::resizeEvent(event);
}

/**
 * @brief Chart::paintEvent
 * @param event
 */
void Chart::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

    // NOTE: This assumes that the data is ordered in ASCENDING order time-wise

    QPainter painter(this);
    painter.setFont(font());
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, false);
    painter.fillRect(rect(), backgroundColor_);

	clearHoveredLists();

    // NOTE: If we decide to display multiple kinds of series in one chart, we'll need to render the hovered one last
    for (const auto& series : series_pointers_) {
        paintSeries(painter, series);
    }

    outlineHoveredData(painter);

    if (hovered_) {
        displayDataMinMax(painter);
        painter.setPen(QPen(textColor_, 2.0));
        painter.drawEllipse(mapFromGlobal(cursor().pos()), 4, 4);
    }

    // NOTE: Don't use rect.bottom (rect.bottom = rect.top + rect.height - 1)
    // Draw horizontal grid lines
    painter.setPen(defaultRectPen_);
    painter.drawLine(0, 0, rect().right(), 0);
    painter.drawLine(0, height(), rect().right(), height());
}

/**
 * @brief Chart::paintSeries
 * @param painter
 * @param series
 */
void Chart::paintSeries(QPainter& painter, const QPointer<const EventSeries>& series)
{
    if (series.isNull()) {
        return;
    }
    const auto series_kind = series->getKind();
    switch (series_kind) {
        case ChartDataKind::PORT_LIFECYCLE:
            paintPortLifecycleSeries(painter, (PortLifecycleEventSeries*) series.data());
            break;
        case ChartDataKind::PORT_EVENT:
            paintPortEventSeries(painter, (PortEventSeries*) series.data());
            break;
        case ChartDataKind::WORKLOAD:
            paintWorkloadEventSeries(painter, (WorkloadEventSeries*) series.data());
            break;
        case ChartDataKind::MARKER:
            paintMarkerEventSeries(painter, (MarkerEventSeries*) series.data());
            break;
        case ChartDataKind::CPU_UTILISATION:
            paintCPUUtilisationSeries(painter, (CPUUtilisationEventSeries*) series.data());
            break;
        case ChartDataKind::MEMORY_UTILISATION:
            paintMemoryUtilisationSeries(painter, (MemoryUtilisationEventSeries*) series.data());
            break;
        case ChartDataKind::NETWORK_UTILISATION: {
            paintNetworkUtilisationSeries(painter, (NetworkUtilisationEventSeries*) series.data());
            break;
        }
        default:
            qWarning("Chart::paintSeries - Series kind not handled");
            break;
    }
}

/**
 * @brief Chart::outlineHoveredData
 * @param painter
 */
void Chart::outlineHoveredData(QPainter& painter)
{
    painter.save();

    // Outline the highlighted rects - for event series
    painter.setPen(highlightPen_);
    painter.setBrush(Qt::NoBrush);
    for (const auto& rect : hoveredRects_) {
        painter.drawRect(rect.adjusted(-1, -1, 1, 1));
    }

    // Outline the highlighted ellipses - for utilisation series
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(highlightBrush_);
    for (const auto& rect : hoveredEllipseRects_) {
        painter.drawEllipse(rect.adjusted(-BORDER_WIDTH, -BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH));
    }

    painter.restore();
}

/**
 * @brief Chart::displayDataMinMax
 * @param painter
 */
void Chart::displayDataMinMax(QPainter& painter)
{
    if (containsYRange_) {
        painter.save();

        auto minStr = QString::number(floor(dataMinY_ * 100)) + "%";
        auto maxStr = QString::number(ceil(dataMaxY_ * 100)) + "%";
        if (series_pointers_.contains(ChartDataKind::NETWORK_UTILISATION)) {
            minStr = NetworkUtilisationEventSeries::getByteString(dataMinY_);
            maxStr = NetworkUtilisationEventSeries::getByteString(dataMaxY_);
        }

        static const int padding = 5;
        int h = fontMetrics().height() + padding;
        int w = qMax(fontMetrics().horizontalAdvance(minStr), fontMetrics().horizontalAdvance(maxStr)) + padding;

        painter.setPen(textColor_);
        painter.setBrush(hoveredRectColor_);

        QRectF maxRect(width() - w, 0, w, h);
        painter.drawRect(maxRect);
        painter.drawText(maxRect, maxStr, QTextOption(Qt::AlignCenter));

        QRectF minRect(width() - w, height() - h, w, h);
        painter.drawRect(minRect);
        painter.drawText(minRect, minStr, QTextOption(Qt::AlignCenter));

        painter.restore();
    }
}
/*
void Chart::paintEventChart(QPainter& painter, const QVector<QList<Event*>>& bins, const ChartDataKind kind)
{
    double bin_width = getBinWidth();
    int bin_count = getBinCount();
    int y = rect().center().y() - bin_width / 2.0;

    for (int i = 0; i < bin_count; i++) {
        int count = bins[i].count();
        if (count == 0)
            continue;
        QRectF rect(i * bin_width, y, bin_width, bin_width);
        if (count == 1) {
            auto event = (PortLifecycleEvent*) bins[i][0];
            if (rectHovered(ChartDataKind ::PORT_LIFECYCLE, rect)) {
                painter.fillRect(rect, highlightBrush_);
            }
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawPixmap(rect.toRect(), lifeCycleTypePixmaps_.value(event->getType()));
            painter.setRenderHint(QPainter::Antialiasing, false);
        } else {
            QString countStr = count > 99 ? "𝑛" : QString::number(count);
            drawTextRect(painter, rect, countStr, portLifecycleColor_.darker(100 + (50 * (count - 1))), ChartDataKind::PORT_LIFECYCLE);
        }
    }

    painter.restore();
}
*/

/**
 * @brief Chart::paintPortLifecycleSeries
 * @param painter
 * @param series
 */
void Chart::paintPortLifecycleSeries(QPainter& painter, const QPointer<const PortLifecycleEventSeries>& series)
{
    if (series.isNull()) {
        return;
    }

    double bin_width = getBinWidth(BIN_WIDTH);
    int bin_count = getBinCount(BIN_WIDTH);
    int y = rect().center().y() - bin_width / 2.0;

    const auto& events = series->getEvents();
    auto bins = binEvents(events,
                          bin_count,
                          series->getFirstAfterTime(getDisplayMin()),
                          series->getFirstAfterTime(getDisplayMax()));

    painter.save();
    painter.setOpacity(portLifecycleSeriesOpacity_);

    for (int i = 0; i < bins.size(); i++) {
        int count = bins[i].count();
        if (count == 0) {
            continue;
        }
        QRectF rect(i * bin_width, y, bin_width, bin_width);
        if (count == 1) {
            auto event = (PortLifecycleEvent*) bins[i][0];
            if (rectHovered(ChartDataKind ::PORT_LIFECYCLE, rect)) {
                painter.fillRect(rect, highlightBrush_);
            }
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawPixmap(rect.toRect(), lifeCycleTypePixmaps_.value(event->getType()));
            painter.setRenderHint(QPainter::Antialiasing, false);
        } else {
            QString countStr = count > 99 ? "𝑛" : QString::number(count);
            drawTextRect(painter, rect, countStr, portLifecycleColor_.darker(100 + (50 * (count - 1))), ChartDataKind::PORT_LIFECYCLE);
        }
    }

    painter.restore();
}

/**
 * @brief Chart::paintPortEventSeries
 * @param painter
 * @param series
 */
void Chart::paintPortEventSeries(QPainter& painter, const QPointer<const PortEventSeries>& series)
{
    if (series.isNull()) {
        return;
    }

    double bin_width = getBinWidth(BIN_WIDTH);
    int bin_count = getBinCount(BIN_WIDTH);
    int y = rect().center().y() - bin_width / 2.0;

    const auto& events = series->getEvents();
    auto bins = binEvents(events,
                          bin_count,
                          series->getFirstAfterTime(getDisplayMin()),
                          series->getFirstAfterTime(getDisplayMax()));

    painter.save();
    painter.setOpacity(portEventSeriesOpacity_);

    for (int i = 0; i < bins.size(); i++) {
        int count = bins[i].count();
        if (count == 0) {
            continue;
        }
        QRectF rect(i * bin_width, y, bin_width, bin_width);
        if (count == 1) {
            auto event = (PortEvent*) bins[i][0];
            if (rectHovered(ChartDataKind::PORT_EVENT, rect)) {
                painter.fillRect(rect, highlightBrush_);
            }
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawPixmap(rect.toRect(), portEventTypePixmaps_.value(event->getType()));
            painter.setRenderHint(QPainter::Antialiasing, false);
        } else {
            QString countStr = count > 99 ? "𝑛" : QString::number(count);
            drawTextRect(painter, rect, countStr, portEventColor_.darker(100 + (50 * (count - 1))), ChartDataKind::PORT_EVENT);
        }
    }

    painter.restore();
}

/**
 * @brief Chart::paintWorkloadEventSeries
 * @param painter
 * @param series
 */
void Chart::paintWorkloadEventSeries(QPainter &painter, const QPointer<const WorkloadEventSeries>& series)
{
    if (series.isNull()) {
        return;
    }

    double bin_width = getBinWidth(BIN_WIDTH);
    int bin_count = getBinCount(BIN_WIDTH);
    int y = rect().center().y() - bin_width / 2.0;

    const auto& events = series->getEvents();
    auto bins = binEvents(events,
                          bin_count,
                          series->getFirstAfterTime(getDisplayMin()),
                          series->getFirstAfterTime(getDisplayMax()));

    painter.save();
    painter.setOpacity(workloadSeriesOpacity_);

    for (int i = 0; i < bins.size(); i++) {
        int count = bins[i].count();
        if (count == 0) {
            continue;
        }
        QRectF rect(i * bin_width, y, bin_width, bin_width);
        if (count == 1) {
            auto event = (WorkloadEvent*) bins[i][0];
            if (rectHovered(ChartDataKind ::WORKLOAD, rect)) {
                painter.fillRect(rect, highlightBrush_);
            }
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawPixmap(rect.toRect(), workloadEventTypePixmaps_.value(event->getType()));
            painter.setRenderHint(QPainter::Antialiasing, false);
        } else {
            QString countStr = count > 99 ? "𝑛" : QString::number(count);
            drawTextRect(painter, rect, countStr, workloadColor_.darker(100 + (50 * (count - 1))), ChartDataKind::WORKLOAD);
        }
    }

    painter.restore();
}

/**
 * @brief Chart::paintMarkerEventSeries
 * @param painter
 * @param series
 */
void Chart::paintMarkerEventSeries(QPainter& painter, const QPointer<const MarkerEventSeries>& series)
{
    if (series.isNull()) {
        return;
    }

    int bin_count = getBinCount(BIN_WIDTH);
    QVector<double> bins(bin_count);
    QVector<double> bin_end_times;
    bin_end_times.reserve(bin_count);

    auto display_min = getDisplayMin();
    auto display_max = getDisplayMax();

    double bin_time_width = (display_max - display_min) / bin_count;
    double current_bin_start_time = display_min;
    for (int i = 0; i < bin_count; i++) {
        bin_end_times.append(current_bin_start_time + bin_time_width);
        current_bin_start_time = bin_end_times.last();
    }

    const auto& start_times_map = series->getMarkerIDsMappedByStartTimes();
    const auto& id_set_duration = series->getMarkerIDSetDurations();
    const auto& start_times = start_times_map.keys();

    auto current_bin_index = 0;
    auto current_bin_itr = bin_end_times.constBegin();

    while (current_bin_itr != bin_end_times.constEnd()) {

        auto current_bin_end_time = *current_bin_itr;
        auto total_duration = 0.0;
        auto number_of_id_sets = 0;

        for (auto start_time_itr = start_times.constBegin(); start_time_itr != start_times.constEnd(); start_time_itr++) {
            const auto& start_time = *start_time_itr;
            // Skip start times that are out of the display range
            if (start_time < display_min) {
                continue;
            }
            if (start_time >= display_max) {
                break;
            }
            if (start_time > current_bin_end_time) {
                break;
            }
            // Calculate average duration
            const auto& IDSetsAtStartTime = start_times_map.value(start_time);
            for (auto ID : IDSetsAtStartTime) {
                total_duration += id_set_duration.value(ID);
                number_of_id_sets++;
            }
        }

        // Store the average duration at the current bin
        if (number_of_id_sets > 0) {
            bins[current_bin_index] = total_duration / number_of_id_sets;
        } else {
            bins[current_bin_index] = 0;
        }

        current_bin_itr++;
        current_bin_index++;

        if (current_bin_index >= bin_count) {
            break;
        }
    }

    // get the maximum duration
    auto maxDuration = 0.0;
    for (int i = 0; i < bins.count(); i++) {
        maxDuration = qMax(bins[i], maxDuration);
    }

    painter.save();
    painter.setOpacity(markerSeriesOpacity_);

    QColor brushColor = markerColor_;
    auto bin_width = getBinWidth(BIN_WIDTH);
    auto availableHeight = height() - BORDER_WIDTH;

    for (int i = 0; i < bins.size(); i++) {
        auto durationMS = bins[i];
        if (durationMS == 0) {
            continue;
        }
        auto rectHeight = (maxDuration <= 0) ? availableHeight : durationMS / maxDuration * availableHeight;
        if (durationMS == -1) {
            rectHeight = 2.0;
        }
        auto y = availableHeight - rectHeight + BORDER_WIDTH / 2.0;
        QRectF rect(i * bin_width, y, bin_width, rectHeight);
        if (rectHovered(ChartDataKind::MARKER, rect)) {
            painter.fillRect(rect, highlightBrush_);
        } else {
            brushColor = (maxDuration <= 0) ? markerColor_.darker(150) : markerColor_.darker(100 + (50 * durationMS / maxDuration));
            painter.setPen(defaultRectPen_);
            painter.setBrush(brushColor);
            painter.drawRect(rect);
        }
    }

    painter.restore();
}

/**
 * @brief Chart::paintCPUUtilisationSeries
 * @param painter
 * @param series
 */
void Chart::paintCPUUtilisationSeries(QPainter& painter, const QPointer<const CPUUtilisationEventSeries>& series)
{
    if (series.isNull()) {
        return;
    }

    const auto& events = series->getEvents();
    const auto& outer_bounds = getOuterDisplayIterators(events, POINT_WIDTH);

    const auto& outer_bound_itrs = outer_bounds.second;
    auto first_event_itr = outer_bound_itrs.first;
    auto last_event_itr = outer_bound_itrs.second;

    // If the first iterator is the same as the last iterator, it means that all the events are out of range; return
    if (first_event_itr == last_event_itr) {
        return;
    }

    const auto& outer_bin_counts = outer_bounds.first;
    auto bin_width = getBinWidth(POINT_WIDTH);
    int bin_count = outer_bin_counts.first + getBinCount(POINT_WIDTH) + outer_bin_counts.second;
    auto bins = binEvents(events, bin_count, first_event_itr, last_event_itr);

    auto availableHeight = height() - bin_width - BORDER_WIDTH / 2.0;
    QList<QRectF> rects;

    for (int i = 0; i < bins.size(); i++) {
        int count = bins[i].count();
        if (count == 0) {
            continue;
        }

        // Calculate the bucket's average utilisation
        auto utilisation = 0.0;
        for (auto e : bins[i]) {
            utilisation += ((CPUUtilisationEvent*)e)->getUtilisation();
        }
        utilisation /= count;

        auto&& y = (1 - utilisation) * availableHeight;
        auto&& x = (i - outer_bin_counts.first) * bin_width;
        rects.append(QRectF(x, y, bin_width, bin_width));
    }

    drawLineFromRects(painter, rects, utilisationColor_, cpuSeriesOpacity_, ChartDataKind::CPU_UTILISATION);
}

/**
 * @brief Chart::paintMemoryUtilisationSeries
 * @param painter
 * @param series
 */
void Chart::paintMemoryUtilisationSeries(QPainter& painter, const QPointer<const MemoryUtilisationEventSeries>& series)
{
    if (series.isNull()) {
        return;
    }

    const auto& events = series->getEvents();
    const auto& outer_bounds = getOuterDisplayIterators(events, POINT_WIDTH);

    const auto& outer_bound_itrs = outer_bounds.second;
    auto first_event_itr = outer_bound_itrs.first;
    auto last_event_itr = outer_bound_itrs.second;

    // If the first iterator is the same as the last iterator, it means that all the events are out of range; return
    if (first_event_itr == last_event_itr) {
        return;
    }

    const auto& outer_bin_counts = outer_bounds.first;
    auto bin_width = getBinWidth(POINT_WIDTH);
    int bin_count = outer_bin_counts.first + getBinCount(POINT_WIDTH) + outer_bin_counts.second;
    auto bins = binEvents(events, bin_count, first_event_itr, last_event_itr);

    auto availableHeight = height() - bin_width - BORDER_WIDTH / 2.0;
    QList<QRectF> rects;

    for (int i = 0; i < bins.size(); i++) {
        int count = bins[i].count();
        if (count == 0) {
            continue;
        }

        // Calculate the bucket's average utilisation
        auto utilisation = 0.0;
        for (auto e : bins[i]) {
            utilisation += ((MemoryUtilisationEvent*)e)->getUtilisation();
        }
        utilisation /= count;

        auto&& y = (1 - utilisation) * availableHeight;
        auto&& x = (i - outer_bin_counts.first) * bin_width;
        rects.append(QRectF(x, y, bin_width, bin_width));
    }

    drawLineFromRects(painter, rects, memoryColor_, memorySeriesOpacity_, ChartDataKind::MEMORY_UTILISATION);
}

/**
 * @brief Chart::paintNetworkUtilisationSeries
 * @param painter
 * @param series
 */
void Chart::paintNetworkUtilisationSeries(QPainter &painter, const QPointer<const NetworkUtilisationEventSeries> &series)
{
    if (series.isNull()) {
        return;
    }

    const auto& events = series->getEvents();
    const auto& outer_bounds = getOuterDisplayIterators(events, POINT_WIDTH);

    const auto& outer_bound_itrs = outer_bounds.second;
    auto first_event_itr = outer_bound_itrs.first;
    auto last_event_itr = outer_bound_itrs.second;

    // If the first iterator is the same as the last iterator, it means that all the events are out of range; return
    if (first_event_itr == last_event_itr) {
        return;
    }

    const auto& outer_bin_counts = outer_bounds.first;
    auto bin_width = getBinWidth(POINT_WIDTH);
    int bin_count = outer_bin_counts.first + getBinCount(POINT_WIDTH) + outer_bin_counts.second;
    auto bins = binEvents(events, bin_count, first_event_itr, last_event_itr);

    QList<QRectF> rects_sent, rects_received;
    auto availableHeight = height() - bin_width - BORDER_WIDTH / 2.0;
    double y_range = dataMaxY_ - dataMinY_;

    for (int i = 0; i < bin_count; i++) {
        int count = bins[i].count();
        if (count == 0) {
            continue;
        }

        // Calculate the bucket's average bytes sent/received
        auto bytes_sent = 0.0;
        auto bytes_received = 0.0;
        for (auto e : bins[i]) {
            auto n_e = (NetworkUtilisationEvent*) e;
            bytes_sent += n_e->getBytesSent();
            bytes_received += n_e->getBytesReceived();
        }
        bytes_sent /= count;
        bytes_received /= count;

        auto x = (i - outer_bin_counts.first) * bin_width;
        auto y_sent = availableHeight - (bytes_sent - dataMinY_) / y_range * availableHeight;
        auto y_received = availableHeight - (bytes_received - dataMinY_) / y_range * availableHeight;
        rects_sent.append(QRectF(x, y_sent, bin_width, bin_width));
        rects_received.append(QRectF(x, y_received, bin_width, bin_width));
    }

    if (rects_sent == rects_received) {
        drawLineFromRects(painter, rects_sent, networkColor_combined_, networkSeriesOpacity_,
                          ChartDataKind::NETWORK_UTILISATION);
    } else {
        drawLineFromRects(painter, rects_sent, networkColor_sent_, networkSeriesOpacity_,
                          ChartDataKind::NETWORK_UTILISATION);
        drawLineFromRects(painter, rects_received, networkColor_received_, networkSeriesOpacity_,
                          ChartDataKind::NETWORK_UTILISATION);
    }
}

/**
 * Chart::getOuterDisplayIterators
 * This gets the iterators of the first and last paint contributing event just outside of the display range
 * It also returns the number of bins to those events either side of the display
 * @param events
 * @param target_bin_width
 * @return a pair of pairs
 *         pair.first = bin counts left/right of the display upto the first/last event that can contribute to paint
 *         pair.second = iterators to the first/last event that can contribute to paint on either side of the display
 */
QPair<QPair<int, int>, QPair<QList<Event*>::const_iterator, QList<Event*>::const_iterator>>
Chart::getOuterDisplayIterators(const QList<Event*>& events, double target_bin_width) const
{
    auto display_min = getDisplayMin();
    auto display_max = getDisplayMax();

    // Get the iterators to the left and right of the display range
    // lower_bound returns itr to first element >= value
    // upper_bound returns itr to first element > value
    auto first_itr = std::lower_bound(events.constBegin(), events.constEnd(), display_min, [](const Event* e, const qint64 &time) {
        return e->getTimeMS() < time;
    });
    auto last_itr = std::upper_bound(events.constBegin(), events.constEnd(), display_max, [](const qint64 &time, const Event* e) {
        return time < e->getTimeMS();
    });

    int bin_count = getBinCount(target_bin_width);
    double bin_time_width = (display_max - display_min) / bin_count;

    // Get the iterator of the leftmost event within the first bin that contributes to drawing
    if (first_itr != events.constBegin()) {
        first_itr -= 1;
    }

    int pre_bin_count = 0;
    auto firstEventTime = (*first_itr)->getTimeMS();

    if (firstEventTime < display_min) {
        pre_bin_count = ceil((display_min - firstEventTime) / bin_time_width);
        auto firstEndTime = display_min - pre_bin_count * bin_time_width;
        for (; first_itr != events.constBegin(); --first_itr) {
            const auto& current_time = (*first_itr)->getTimeMS();
            // Keep going until we overshoot, then move back if we can
            if (current_time < firstEndTime) {
                first_itr++;
                break;
            }
        }
    }

    // Get the iterator of the rightmost event within the last bin that contributes to drawing
    if (last_itr == events.constEnd()) {
        return {{pre_bin_count, 0}, {first_itr, last_itr}};
    }

    int post_bin_count = 0;
    auto lastEventTime = (*last_itr)->getTimeMS();

    if (lastEventTime > display_max) {
        post_bin_count = ceil((lastEventTime - display_max) / bin_time_width);
        auto lastEndTime = display_max + post_bin_count * bin_time_width;
        for (; last_itr != events.constEnd(); ++last_itr) {
            const auto& current_time = (*last_itr)->getTimeMS();
            // Keep going until we overshoot, then move back if we can
            if (current_time >= lastEndTime) {
                if (last_itr != events.constBegin()) {
                    last_itr--;
                    break;
                }
            }
        }
    }

    return {{pre_bin_count, post_bin_count}, {first_itr, last_itr}};
}

/**
 * @brief Chart::binEvents
 * This puts the events from the first to last iterator into the number of bins provided within the display range
 * @param events
 * @param bin_count
 * @param first
 * @param last
 * @return
 */
QVector<QList<Event*>> Chart::binEvents(const QList<Event*>& events,
                                        int bin_count,
                                        QList<Event*>::const_iterator first,
                                        QList<Event*>::const_iterator last) const
{
    if (first == events.constEnd()) {
        return {};
    }

    QVector<QList<Event*>> bins(bin_count);
    QVector<double> bin_end_times;
    bin_end_times.reserve(bin_count);

    auto display_min = getDisplayMin();
    auto display_max = getDisplayMax();

    // Calculate the bin end times
    double bin_time_width = (display_max - display_min) / bin_count;
    double current_bin_start_time = display_min;
    for (int i = 0; i < bin_count; i++) {
        bin_end_times.append(current_bin_start_time + bin_time_width);
        current_bin_start_time = bin_end_times.last();
    }

    // Put the event in the correct bin
    auto current_bin = 0;
    auto current_bin_itr = bin_end_times.constBegin();
    auto end_bin_itr = bin_end_times.constEnd();
    for (auto current = first; current != events.constEnd(); current++) {
        const auto& event = (*current);
        const auto& event_time = event->getTimeMS();
        if (event_time < display_min || event_time >= display_max) {
            continue;
        }
        while (current_bin_itr != end_bin_itr) {
            const auto& bin_end_time = *current_bin_itr;
            if (event_time >= bin_end_time) {
                current_bin_itr++;
                current_bin++;
            } else {
                break;
            }
        }
        if (current_bin < bin_count) {
            bins[current_bin].append(event);
        }
        if (current == last) {
            break;
        }
    }

    return bins;
}

namespace AggServerResponse
{
	inline uint qHash(const LifecycleType& key, uint seed)
	{
		return ::qHash(static_cast<uint>(key), seed);
	}
}

/**
 * @brief Chart::drawTextRect
 * @param painter
 * @param rect
 * @param text
 * @param color
 * @param series_kind
 */
void Chart::drawTextRect(QPainter& painter, const QRectF& rect, const QString& text, const QColor& color, ChartDataKind series_kind)
{
    painter.save();

    if (rectHovered(series_kind, rect)) {
        painter.fillRect(rect, highlightBrush_);
        painter.setPen(defaultTextPen_);
    } else {
        painter.setPen(defaultRectPen_);
        painter.setBrush(color);
        painter.drawRect(rect);
        painter.setPen(QPen(getContrastingColor(color), 2.0));
    }

    painter.drawText(rect, text, QTextOption(Qt::AlignCenter));
    painter.restore();
}


/**
 * @brief Chart::drawLineFromRects
 * @param rects
 * @param color
 * @param opacity
 * @param series_kind
 */
void Chart::drawLineFromRects(QPainter& painter, const QList<QRectF>& rects, const QColor& color, double opacity, ChartDataKind series_kind)
{
    if (rects.isEmpty()) {
        return;
    }

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity(opacity);
    painter.setPen(defaultEllipsePen_);
    painter.setBrush(color);

    if (rects.size() == 1) {
        auto rect = rects.first();
        painter.drawEllipse(rect);
        rectHovered(series_kind, rect);
    } else {
        QPen linePen(color, 3.0);
        for (int i = 1; i < rects.count(); i++) {
            auto rect1 = rects.at(i - 1);
            auto rect2 = rects.at(i);
            painter.setPen(linePen);
            painter.drawLine(rect1.center(), rect2.center());
            painter.setPen(defaultEllipsePen_);
            painter.drawEllipse(rect1);
            painter.drawEllipse(rect2);
            rectHovered(series_kind, rect1);
            rectHovered(series_kind, rect2);
        }
    }

    painter.restore();
}

/**
 * @brief Chart::rectHovered
 * @param kind
 * @param hitRect
 * @return
 */
bool Chart::rectHovered(ChartDataKind kind, const QRectF& hitRect)
{
    auto painterRect = hitRect.adjusted(-PEN_WIDTH / 2.0, 0, PEN_WIDTH / 2.0, 0);
    if (rectHovered(painterRect)) {
        // If the hit rect is hovered, store/update the hovered time range for the provided series kind
        QPair<qint64, qint64> timeRange = {mapPixelToTime(hitRect.left()), mapPixelToTime(hitRect.right())};
        if (hoveredSeriesTimeRange_.contains(kind)) {
            timeRange.first = hoveredSeriesTimeRange_.value(kind).first;
        }
        hoveredSeriesTimeRange_[kind] = timeRange;
        if (kind == ChartDataKind::CPU_UTILISATION || kind == ChartDataKind::MEMORY_UTILISATION || kind == ChartDataKind::NETWORK_UTILISATION) {
            hoveredEllipseRects_.append(hitRect);
        } else {
            hoveredRects_.append(hitRect);
        }
        return true;
    }
    return false;
}

/**
 * @brief Chart::rectHovered
 * @param hitRect
 * @return
 */
bool Chart::rectHovered(const QRectF &hitRect)
{
	return hoveredRect_.intersects(hitRect);
}

/**
 * @brief Chart::clearHoveredLists
 */
void Chart::clearHoveredLists()
{
	// clear previously hovered series kinds
	hoveredSeriesTimeRange_.clear();
	hoveredEllipseRects_.clear();
	hoveredRects_.clear();
}

/**
 * @brief Chart::updateBinnedData
 * @param kind
 */
void Chart::updateBinnedData(ChartDataKind kind)
{
    // TODO: Part of binning work
	/*if (!seriesList_.contains(kind))
		return;

	auto dataRange = dataMaxX_ - dataMinX_;
	auto displayRange = displayMax_ - displayMin_;

	auto binRatio =  BIN_WIDTH / (double) width();
	binTimeWidth_ = binRatio * displayRange;
	binCount_ = ceil(dataRange / binTimeWidth_);
	binPixelWidth_ = (double) width() / binCount_;

	// clear binned data
	auto& binnedData = getBinnedData(kind);
	binnedData.clear();
	binnedData.resize(binCount_);

	QVector<double> binEndTimes;
	binEndTimes.reserve(binCount_);

	// calculate the bin end times for the whole data range
	auto currentTime = dataMinX_;
	for (auto i = 0; i < binCount_; i++) {
		binEndTimes.append(currentTime + binTimeWidth_);
		currentTime = binEndTimes.last();
	}

	for (auto series : seriesList_.values(kind)) {
		if (!series)
			continue;

		auto currentBin = 0;
		auto currentBinItr = binEndTimes.constBegin();
		auto endBinItr = binEndTimes.constEnd();
		const auto& events = series->getEvents();

		// put the data in the correct bin
		for (auto eventItr = events.constBegin(); eventItr != events.constEnd(); eventItr++) {
			const auto& eventTime = (*eventItr)->getTimeMS();
			while (currentBinItr != endBinItr) {
				auto currentBinEndTime = *currentBinItr;
				if (eventTime > currentBinEndTime) {
					currentBinItr++;
					currentBin++;
				} else {
					break;
				}
			}
			if (currentBin < binCount_) {
				binnedData[currentBin].append(*eventItr);
			}
		}
	}*/
}

/**
 * @brief Chart::updateSeriesPixmaps
 */
void Chart::updateSeriesPixmaps()
{
	Theme* theme = Theme::theme();
	bool colorLifecyclePixmaps = false;
	bool colorWorkerPixmaps = false;
	bool colorPortEventPixmaps = false;

	switch (hoveredSeriesKind_) {
		case ChartDataKind::DATA: {
			colorLifecyclePixmaps = true;
			colorWorkerPixmaps = true;
			colorPortEventPixmaps = true;
			break;
		}
		case ChartDataKind::PORT_LIFECYCLE:
			colorLifecyclePixmaps = true;
			break;
		case ChartDataKind::WORKLOAD:
			colorWorkerPixmaps = true;
			break;
		case ChartDataKind::PORT_EVENT:
			colorPortEventPixmaps = true;
			break;
		default:
			return;
	}

    if (colorLifecyclePixmaps) {
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::NO_TYPE] = theme->getImage("Icons", "circleQuestion", QSize(), theme->getAltTextColor());
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::CONFIGURE] = theme->getImage("Icons", "gear", QSize(), theme->getTextColor());
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::ACTIVATE] = theme->getImage("Icons", "power", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::PASSIVATE] = theme->getImage("Icons", "bed", QSize(), QColor(255, 179, 102));
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::TERMINATE] = theme->getImage("Icons", "cancel", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
    } else {
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::NO_TYPE] = theme->getImage("Icons", "circleQuestion", QSize(), backgroundColor_);
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::CONFIGURE] = theme->getImage("Icons", "gear", QSize(), backgroundColor_);
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::ACTIVATE] = theme->getImage("Icons", "power", QSize(), backgroundColor_);
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::PASSIVATE] = theme->getImage("Icons", "bed", QSize(), backgroundColor_);
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::TERMINATE] = theme->getImage("Icons", "cancel", QSize(), backgroundColor_);
    }

	if (colorWorkerPixmaps) {
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::STARTED] = theme->getImage("Icons", "play", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::FINISHED] = theme->getImage("Icons", "avStop", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleFilled", QSize(), QColor(72, 151, 189));
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::WARNING] = theme->getImage("Icons", "triangleCritical", QSize(), theme->getSeverityColor(Notification::Severity::WARNING));
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::ERROR_EVENT] = theme->getImage("Icons", "circleCrossDark", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MARKER] = theme->getImage("Icons", "bookmarkTwoTone", QSize(), QColor(72, 151, 199));
	} else {
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::STARTED] = theme->getImage("Icons", "play", QSize(), backgroundColor_);
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::FINISHED] = theme->getImage("Icons", "avStop", QSize(), backgroundColor_);
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleFilled", QSize(), backgroundColor_);
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::WARNING] = theme->getImage("Icons", "triangleCritical", QSize(), backgroundColor_);
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::ERROR_EVENT] = theme->getImage("Icons", "circleCrossDark", QSize(), backgroundColor_);
		workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MARKER] = theme->getImage("Icons", "bookmarkTwoTone", QSize(), backgroundColor_);
	}

	if (colorPortEventPixmaps) {
		portEventTypePixmaps_[PortEvent::PortEventType::SENT] = theme->getImage("Icons", "arrowTopRight", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
		portEventTypePixmaps_[PortEvent::PortEventType::RECEIVED] = theme->getImage("Icons", "arrowBottomRight", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
		portEventTypePixmaps_[PortEvent::PortEventType::STARTED_FUNC] = theme->getImage("Icons", "arrowLineLeft", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
		portEventTypePixmaps_[PortEvent::PortEventType::FINISHED_FUNC] = theme->getImage("Icons", "arrowToLineRight", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
		portEventTypePixmaps_[PortEvent::PortEventType::IGNORED] = theme->getImage("Icons", "circleCross");
		portEventTypePixmaps_[PortEvent::PortEventType::EXCEPTION] = theme->getImage("Icons", "circleCritical");
		portEventTypePixmaps_[PortEvent::PortEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleMessage");
	} else {
		portEventTypePixmaps_[PortEvent::PortEventType::SENT] = theme->getImage("Icons", "arrowTopRight", QSize(), backgroundColor_);
		portEventTypePixmaps_[PortEvent::PortEventType::RECEIVED] = theme->getImage("Icons", "arrowBottomRight", QSize(), backgroundColor_);
		portEventTypePixmaps_[PortEvent::PortEventType::STARTED_FUNC] = theme->getImage("Icons", "arrowLineLeft", QSize(), backgroundColor_);
		portEventTypePixmaps_[PortEvent::PortEventType::FINISHED_FUNC] = theme->getImage("Icons", "arrowToLineRight", QSize(), backgroundColor_);
		portEventTypePixmaps_[PortEvent::PortEventType::IGNORED] = theme->getImage("Icons", "circleCross", QSize(), backgroundColor_);
		portEventTypePixmaps_[PortEvent::PortEventType::EXCEPTION] = theme->getImage("Icons", "circleCritical", QSize(), backgroundColor_);
		portEventTypePixmaps_[PortEvent::PortEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleMessage", QSize(), backgroundColor_);
	}
}

/**
 * @brief Chart::getContrastingColor
 * @param color
 * @return
 */
QColor Chart::getContrastingColor(const QColor &color)
{
	auto hue = color.hue();
	auto saturation = color.saturation() < 128 ? 255 : 0;
	auto value = color.value() < 128 ? 255 : 0;

	QColor contrastColor;
	contrastColor.setHsv(hue, saturation, value);
	return contrastColor;
}

/**
 * @brief Chart::getBinCount
 * @param target_bin_width
 * @return
 */
int Chart::getBinCount(double target_bin_width) const
{
    return ceil( (double)width() / target_bin_width );
}

/**
 * @brief Chart::getBinWidth
 * @return
 */
double Chart::getBinWidth(double target_bin_width) const
{
    return (double)width() / getBinCount(target_bin_width);
}

/**
 * @brief Chart::getDisplayMin
 * @return
 */
double Chart::getDisplayMin() const
{
    return (dataMaxX_ - dataMinX_) * minRatio_ + dataMinX_;
}

/**
 * @brief Chart::getDisplayMax
 * @return
 */
double Chart::getDisplayMax() const
{
    return (dataMaxX_ - dataMinX_) * maxRatio_ + dataMinX_;
}

/**
 * @brief Chart::mapPixelToTime
 * @param x
 * @return
 */
qint64 Chart::mapPixelToTime(double x)
{
	auto timeRange = getDisplayMax() - getDisplayMin();
	auto ratio = x / width();
	return getDisplayMin() + timeRange * ratio;
}

/**
 * @brief Chart::mapTimeToPixel
 * @param time
 * @return
 */
double Chart::mapTimeToPixel(double time)
{
	auto timeRange = getDisplayMax() - getDisplayMin();
	auto adjustedTime = time - getDisplayMin();
	return adjustedTime / timeRange * width();
}