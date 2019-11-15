#include "chart.h"
#include "../../../../theme.h"
#include "../../Data/Events/portlifecycleevent.h"
#include "../../Data/Events/cpuutilisationevent.h"
#include "../../Data/Events/memoryutilisationevent.h"
#include "../../Data/Events/markerevent.h"
#include "../../Data/Series/markereventseries.h"

#include <QPainter>
#include <QPainter>
#include <algorithm>
#include <iostream>

#define BACKGROUND_OPACITY 0.25
#define BORDER_WIDTH 2.0

#define PEN_WIDTH 1.0
#define BIN_WIDTH 22.0
#define POINT_WIDTH 8.0

#define PRINT_RENDER_TIMES false

using namespace MEDEA;

/**
 * @brief Chart::Chart
 * @param experimentRunID
 * @param experimentStartTime
 * @param parent
 */
Chart::Chart(quint32 experimentRunID, qint64 experimentStartTime, QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    experimentRunID_ = experimentRunID;
    experimentRunStartTime_ = experimentStartTime;
    hoveredSeriesKind_ = ChartDataKind::DATA;

    dataMinX_ = DBL_MAX;
    dataMaxX_ = DBL_MIN;
    dataMinY_ = DBL_MAX;
    dataMaxY_ = DBL_MIN;

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
void Chart::addSeries(EventSeries* series)
{
    if (series) {
        seriesList_[series->getKind()] = series;
        if (series->getKind() == ChartDataKind::CPU_UTILISATION ||
                series->getKind() == ChartDataKind::MEMORY_UTILISATION) {
            containsYRange_ = true;
            connect(series, &EventSeries::minYValueChanged, this, &Chart::updateVerticalMin);
            connect(series, &EventSeries::maxYValueChanged, this, &Chart::updateVerticalMax);
        }
    }
}


/**
 * @brief Chart::removeSeries
 * @param kind
 */
void Chart::removeSeries(ChartDataKind kind)
{
    seriesList_.remove(kind);
}


/**
 * @brief Chart::getSeries
 * @return
 */
const QHash<ChartDataKind, EventSeries*>& Chart::getSeries() const
{
    return seriesList_;
}


/**
 * @brief Chart::getHovereSeriesKinds
 * @return
 */
const QList<ChartDataKind> Chart::getHovereSeriesKinds() const
{
    return hoveredSeriesTimeRange_.keys();
}


/**
 * @brief Chart::getHoveredTimeRange
 * @param kind
 * @return
 */
const QPair<qint64, qint64> Chart::getHoveredTimeRange(ChartDataKind kind) const
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

    displayMin_ = (dataMaxX_ - dataMinX_) * minRatio_ + dataMinX_;
    displayMax_ = (dataMaxX_ - dataMinX_) * maxRatio_ + dataMinX_;

    update();
}


/**
 * @brief Chart::setDisplayMinRatio
 * @param ratio
 */
void Chart::setDisplayMinRatio(double ratio)
{
    minRatio_ = ratio;
    displayMin_ = (dataMaxX_ - dataMinX_) * ratio + dataMinX_;
    update();
}


/**
 * @brief Chart::setDisplayMaxRatio
 * @param ratio
 */
void Chart::setDisplayMaxRatio(double ratio)
{
    maxRatio_ = ratio;
    displayMax_ = (dataMaxX_ - dataMinX_) * ratio + dataMinX_;
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
    displayMin_ = (dataMaxX_ - dataMinX_) * minRatio + dataMinX_;
    displayMax_ = (dataMaxX_ - dataMinX_) * maxRatio + dataMinX_;
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
void Chart::updateBinnedData(QSet<ChartDataKind> kinds)
{
    return;

    if (kinds.isEmpty())
        kinds = seriesList_.keys().toSet();

    for (auto kind : kinds) {
        updateBinnedData(kind);
    }

    update();
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
bool Chart::isHovered()
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
 * @brief Chart::setSeriesKindVisible
 * @param kind
 * @param visible
 */
void Chart::setSeriesKindVisible(ChartDataKind kind, bool visible)
{
    seriesKindVisible_[kind] = visible;
    update();
}


/**
 * @brief Chart::seriesKindHovered
 * @param kind
 */
void Chart::seriesKindHovered(ChartDataKind kind)
{
    if (kind == hoveredSeriesKind_)
        return;

    portLifecycleColor_ = backgroundColor_;
    workloadColor_ = backgroundColor_;
    utilisationColor_ = backgroundColor_;
    memoryColor_ = backgroundColor_;
    markerColor_ = backgroundColor_;

    double alpha = 0.25;
    portSeriesOpacity_ = alpha;
    workloadSeriesOpacity_ = alpha;
    cpuSeriesOpacity_ = alpha;
    memorySeriesOpacity_ = alpha;
    markerSeriesOpacity_ = alpha;

    switch (kind) {
    case ChartDataKind::PORT_LIFECYCLE: {
        portLifecycleColor_ = defaultPortLifecycleColor_;
        portSeriesOpacity_ = 1.0;
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
    default: {
        // clear hovered state
        portLifecycleColor_ = defaultPortLifecycleColor_;
        workloadColor_ = defaultWorkloadColor_;
        utilisationColor_ = defaultUtilisationColor_;
        memoryColor_ = defaultMemoryColor_;
        markerColor_ = defaultMarkerColor_;
        portSeriesOpacity_ = 1.0;
        workloadSeriesOpacity_ = 1.0;
        cpuSeriesOpacity_ = 1.0;
        memorySeriesOpacity_ = 1.0;
        markerSeriesOpacity_ = 1.0;
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

    defaultPortLifecycleColor_ = Qt::gray;
    defaultWorkloadColor_ = Qt::gray;

    defaultPortLifecycleColor_ = QColor(186,85,211);
    portLifecycleColor_ = defaultPortLifecycleColor_;

    defaultWorkloadColor_ = QColor(0,206,209);
    workloadColor_ = defaultWorkloadColor_;

    defaultUtilisationColor_ = QColor(30,144,255);
    utilisationColor_ = defaultUtilisationColor_;

    //defaultMemoryColor_ = QColor(50,205,50);
    defaultMemoryColor_ = theme->getSeverityColor(Notification::Severity::SUCCESS);
    memoryColor_ = defaultMemoryColor_;

    defaultMarkerColor_ = QColor(221,188,153);
    //defaultMarkerColor_ = QColor(240,128,128);
    markerColor_ = defaultMarkerColor_;

    backgroundDefaultColor_ = theme->getAltBackgroundColor();
    backgroundDefaultColor_.setAlphaF(BACKGROUND_OPACITY);
    backgroundHighlightColor_ = theme->getActiveWidgetBorderColor();
    backgroundHighlightColor_.setAlphaF(BACKGROUND_OPACITY * 2.0);

    textColor_ = theme->getTextColor();
    gridColor_ = theme->getAltTextColor();
    highlightTextColor_ = theme->getTextColor(ColorRole::SELECTED);
    backgroundColor_ = backgroundDefaultColor_;
    highlightColor_ = theme->getHighlightColor();
    hoveredRectColor_ = theme->getActiveWidgetBorderColor();

    defaultRectPen_ = QPen(gridColor_, 0.5);
    defaultEllipsePen_ = QPen(gridColor_, 2.0);
    highlightPen_ = QPen(highlightColor_, 2.0);
    highlightBrush_ = QBrush(getContrastingColor(textColor_));

    messagePixmap_ = theme->getImage("Icons", "exclamation", QSize(), theme->getMenuIconColor());
    markerPixmap_ = theme->getImage("Icons", "bookmark", QSize(), theme->getMenuIconColor());
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

    /*
     * NOTE: This assumes that the data is ordered in ascending order time-wise
     */

    auto start = QDateTime::currentMSecsSinceEpoch();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, false);
    painter.setFont(font());

    painter.fillRect(rect(), backgroundColor_);

    clearHoveredLists();

    // NOTE: If we decide to display multiple kinds of event series in one chart, we'll need to render the hovered one last
    for (const auto& seriesKind : seriesList_.keys()) {
        paintSeries(painter, seriesKind);
    }
    painter.setOpacity(1.0);

    // outline the highlighted rects - for event series
    painter.setPen(highlightPen_);
    painter.setBrush(Qt::NoBrush);
    for (auto rect : hoveredRects_) {
        rect.adjust(-1, -1, 1, 1);
        painter.drawRect(rect);
    }

    // outline the highlighted ellipses - for utilisation series
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(highlightBrush_);
    for (auto rect : hoveredEllipseRects_) {
        rect.adjust(-BORDER_WIDTH, -BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
        painter.drawEllipse(rect);
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(defaultRectPen_);

    // display the y-range and send the series points that were hovered over
    if (hovered_) {
        if (containsYRange_) {
            QString minStr = QString::number(floor(dataMinY_ * 100)) + "%";
            QString maxStr = QString::number(ceil(dataMaxY_ * 100)) + "%";
            int h = fontMetrics().height();
            int w = qMax(fontMetrics().width(minStr), fontMetrics().width(maxStr)) + 5;
            QRectF maxRect(width() - w, 0, w, h);
            QRectF minRect(width() - w, height() - h, w, h);
            painter.setPen(textColor_);
            painter.setBrush(hoveredRectColor_);
            painter.drawRect(maxRect);
            painter.drawRect(minRect);
            painter.drawText(maxRect, maxStr, QTextOption(Qt::AlignCenter));
            painter.drawText(minRect, minStr, QTextOption(Qt::AlignCenter));
        }
        painter.setPen(QPen(textColor_, 2.0));
        painter.drawEllipse(mapFromGlobal(cursor().pos()), 4, 4);
    }

    // NOTE: Don't use rect.bottom (rect.bottom = rect.top + rect.height - 1)
    // draw horizontal grid lines
    painter.drawLine(0, 0, rect().right(), 0);
    painter.drawLine(0, height(), rect().right(), height());

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES) {
        qDebug() << "Total Render Took: " << finish - start << "ms";
        qDebug() << "------------------------------------------------";
    }
}


/**
 * @brief Chart::paintSeries
 * @param painter
 * @param kind
 */
void Chart::paintSeries(QPainter &painter, const ChartDataKind kind)
{
    if (!seriesKindVisible_.value(kind, false))
        return;

    switch (kind) {
    case ChartDataKind::PORT_LIFECYCLE:
        paintPortLifecycleEventSeries(painter);
        break;
    case ChartDataKind::WORKLOAD:
        paintWorkloadEventSeries(painter);
        break;
    case ChartDataKind::CPU_UTILISATION:
        paintCPUUtilisationEventSeries(painter);
        break;
    case ChartDataKind::MEMORY_UTILISATION:
        paintMemoryUtilisationEventSeries(painter);
        break;
    case ChartDataKind::MARKER:
        paintMarkerEventSeries(painter);
        break;
    default:
        qWarning("Chart::paintSeries - Series kind not handled");
        break;
    }
}


namespace AggServerResponse {
/**
 * @brief qHash
 * @param key
 * @param seed
 * @return
 */
inline uint qHash(const LifecycleType& key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
}


/**
 * @brief Chart::paintPortLifecycleEventSeries
 * @param painter
 */
void Chart::paintPortLifecycleEventSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    const auto eventSeries = seriesList_.value(ChartDataKind::PORT_LIFECYCLE, nullptr);
    if (!eventSeries)
        return;


    double barWidth = BIN_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<Event*> > buckets(barCount);
    QVector<double> bucket_endTimes;
    bucket_endTimes.reserve(barCount);

    double barTimeWidth = (displayMax_ - displayMin_) / barCount;
    double current_left = displayMin_;
    for (int i = 0; i < barCount; i++) {
        bucket_endTimes.append(current_left + barTimeWidth);
        current_left = bucket_endTimes.last();
    }

    const auto& events = eventSeries->getEvents();
    auto current = events.constBegin();
    auto upper = events.constEnd();
    for (; current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        if (current_time > displayMin_) {
            break;
        }
    }
//    /qDebug() << "Port series#: " << events.count();

    auto current_bucket = 0;
    auto current_bucket_ittr = bucket_endTimes.constBegin();
    auto end_bucket_ittr = bucket_endTimes.constEnd();

    // put the data in the correct bucket
    for (;current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        while (current_bucket_ittr != end_bucket_ittr) {
            if (current_time > (*current_bucket_ittr)) {
                current_bucket_ittr ++;
                current_bucket ++;
            } else {
                break;
            }
        }
        if (current_bucket < barCount) {
            buckets[current_bucket].append(*current);
        }
    }

    QColor seriesColor = portLifecycleColor_;
    QColor brushColor = seriesColor;
    QPen textPen(textColor_, 2.0);

    int y = rect().center().y() - barWidth / 2.0;
    painter.setOpacity(portSeriesOpacity_);

    for (int i = 0; i < barCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;
        QRectF rect(i * barWidth, y, barWidth, barWidth);
        if (count == 1) {
            auto event = (PortLifecycleEvent*) buckets[i][0];
            if (rectHovered(eventSeries->getKind(), rect)) {
                painter.fillRect(rect, highlightBrush_);
            }
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawPixmap(rect.toRect(), lifeCycleTypePixmaps_.value(event->getType()));
            painter.setRenderHint(QPainter::Antialiasing, false);
        } else {
            if (rectHovered(eventSeries->getKind(), rect)) {
                textPen.setColor(textColor_);
                painter.fillRect(rect, highlightBrush_);
            } else {
                brushColor = seriesColor.darker(100 + (50 * (count - 1)));
                textPen.setColor(getContrastingColor(brushColor));
                painter.setPen(defaultRectPen_);
                painter.setBrush(brushColor);
                painter.drawRect(rect);
            }
            QString countStr = count > 99 ? "𝑛" : QString::number(count);
            painter.setPen(textPen);
            painter.drawText(rect, countStr, QTextOption(Qt::AlignCenter));
        }
    }

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "PORT Render Took: " << finish - start << "ms";
}


/**
 * @brief Chart::paintWorkloadEventSeries
 * @param painter
 */
void Chart::paintWorkloadEventSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    const auto eventSeries = seriesList_.value(ChartDataKind::WORKLOAD, nullptr);
    if (!eventSeries)
        return;

    double barWidth = BIN_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<Event*> > buckets(barCount);
    QVector<double> bucket_endTimes;
    bucket_endTimes.reserve(barCount);

    double barTimeWidth = (displayMax_ - displayMin_) / barCount;
    double current_left = displayMin_;
    for (int i = 0; i < barCount; i++) {
        bucket_endTimes.append(current_left + barTimeWidth);
        current_left = bucket_endTimes.last();
    }

    const auto& events = eventSeries->getEvents();
    auto current = events.constBegin();
    auto upper = events.constEnd();
    for (; current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        if (current_time > displayMin_) {
            break;
        }
    }

    auto current_bucket = 0;
    auto current_bucket_ittr = bucket_endTimes.constBegin();
    auto end_bucket_ittr = bucket_endTimes.constEnd();

    // put the data in the correct bucket
    for (;current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        while (current_bucket_ittr != end_bucket_ittr) {
            if (current_time > (*current_bucket_ittr)) {
                current_bucket_ittr ++;
                current_bucket ++;
            } else {
                break;
            }
        }
        if (current_bucket < barCount) {
            buckets[current_bucket].append(*current);
        }
    }

    QColor seriesColor = workloadColor_;
    QColor brushColor = seriesColor;
    QPen textPen(textColor_, 2.0);

    int y = rect().center().y() - barWidth / 2.0;
    painter.setOpacity(workloadSeriesOpacity_);

    for (int i = 0; i < barCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;
        QRectF rect(i * barWidth, y, barWidth, barWidth);
        if (count == 1) {
            auto event = (WorkloadEvent*) buckets[i][0];
            if (rectHovered(eventSeries->getKind(), rect)) {
                painter.fillRect(rect, highlightBrush_);
            }
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawPixmap(rect.toRect(), workloadEventTypePixmaps_.value(event->getType()));
            painter.setRenderHint(QPainter::Antialiasing, false);
        } else {
            if (rectHovered(eventSeries->getKind(), rect)) {
                textPen.setColor(textColor_);
                painter.fillRect(rect, highlightBrush_);
            } else {
                brushColor = seriesColor.darker(100 + (50 * (count - 1)));
                textPen.setColor(getContrastingColor(brushColor));
                painter.setPen(defaultRectPen_);
                painter.setBrush(brushColor);
                painter.drawRect(rect);
            }
            QString countStr = count > 99 ? "𝑛" : QString::number(count);
            painter.setPen(textPen);
            painter.drawText(rect, countStr, QTextOption(Qt::AlignCenter));
        }
    }

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "WORKLOAD Render Took: " << finish - start << "ms";
}


/**
 * @brief Chart::paintCPUUtilisationEventSeries
 * @param painter
 */
void Chart::paintCPUUtilisationEventSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    const auto eventSeries = seriesList_.value(ChartDataKind::CPU_UTILISATION, nullptr);
    if (!eventSeries)
        return;

    const auto& events = eventSeries->getEvents();
    if  (events.isEmpty())
        return;

    double barWidth = POINT_WIDTH;
    double barCount = ceil((double)width() / barWidth);
    double barTimeWidth = (displayMax_ - displayMin_) / barCount;

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    // get the iterators to the left and right of the display range
    auto firstEventItr = std::lower_bound(events.constBegin(), events.constEnd(), displayMin_, [](const Event* e, const qint64 &time) {
        return e->getTimeMS() < time;
    });
    auto lastEventItr = std::upper_bound(events.constBegin(), events.constEnd(), displayMax_, [](const qint64 &time, const Event* e) {
        return time < e->getTimeMS();
    });

    // if the first iterator is the same as the last iterator, it means that all the events are out of range; return
    if (firstEventItr == lastEventItr) {
        return;
    }

    if (firstEventItr != events.constBegin()) {
        firstEventItr -= 1;
    }

    auto prevBarCount = 0.0;
    auto firstEventTime = (*firstEventItr)->getTimeMS();
    bool firstItrIsToTheLeftOfDisplay = (firstEventTime < displayMin_);
    if (firstItrIsToTheLeftOfDisplay) {
        prevBarCount = ceil((displayMin_ - firstEventTime) / barTimeWidth);
    }

    // get the iterator of the leftmost event up to the first bin that contributes to drawing
    auto first_contributing_event = firstEventItr;
    auto firstEndTime = displayMin_ - prevBarCount * barTimeWidth;
    if (firstItrIsToTheLeftOfDisplay) {
        for (; first_contributing_event != events.constBegin(); --first_contributing_event) {
            const auto& current_time = (*first_contributing_event)->getTimeMS();
            // keep going until we overshoot, then move back if we can
            if (current_time < firstEndTime) {
                first_contributing_event++;
                break;
            }
        }
    }

    auto postBarCount = 0.0;
    bool lastItrIsToTheRightOfDisplay = false;
    if (lastEventItr != events.constEnd()) {
        auto lastEventTime = (*lastEventItr)->getTimeMS();
        lastItrIsToTheRightOfDisplay = (lastEventTime >= displayMax_);
        if (lastItrIsToTheRightOfDisplay) {
            postBarCount = ceil((lastEventTime - displayMax_) / barTimeWidth);
        }
    }

    // get the iterator of the rightmost event up to the last bin that contributes to drawing
    auto last_contributing_event = lastEventItr;
    if (lastItrIsToTheRightOfDisplay) {
        auto lastEndTime = displayMax_ + postBarCount * barTimeWidth;
        for (; last_contributing_event != events.constEnd(); ++last_contributing_event) {
            const auto& current_time = (*last_contributing_event)->getTimeMS();
            // keep going until we overshoot, then move back if we can
            if (current_time >= lastEndTime) {
                if (last_contributing_event != events.constBegin()) {
                    last_contributing_event--;
                    break;
                }
            }
        }
    }

    auto totalBarCount = prevBarCount + barCount + postBarCount;
    auto currentLeft = firstEndTime;

    QVector< QList<CPUUtilisationEvent*> > buckets(totalBarCount);
    QVector<double> bucketEndTimes;
    bucketEndTimes.reserve(totalBarCount);

    // calculate the bucket end times
    for (int i = 0; i < totalBarCount; i++) {
        bucketEndTimes.append(currentLeft + barTimeWidth);
        currentLeft = bucketEndTimes.last();
    }

    auto currentBucketIndex = 0;
    auto currentBucketItr = bucketEndTimes.constBegin();
    auto endBucketItr = bucketEndTimes.constEnd();

    // put the data in the correct bucket
    while (first_contributing_event != events.constEnd()) {
        auto event = (CPUUtilisationEvent*)(*first_contributing_event);
        const auto& currentTime = event->getTimeMS();
        while (currentBucketItr != endBucketItr) {
            if (currentTime >= (*currentBucketItr)) {
                currentBucketItr++;
                currentBucketIndex++;
            } else {
                break;
            }
        }
        if (currentBucketIndex < totalBarCount) {
            buckets[currentBucketIndex].append(event);
        }
        if (first_contributing_event == last_contributing_event) {
            break;
        }
        first_contributing_event++;
    }

    auto availableHeight = height() - barWidth - BORDER_WIDTH / 2.0;
    auto seriesColor = utilisationColor_;
    QList<QRectF> rects;

    for (int i = 0; i < totalBarCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;

        // calculate the bucket's average utilisation
        auto utilisation = 0.0;
        for (auto e : buckets[i]) {
            utilisation += e->getUtilisation();
        }
        utilisation /= count;

        double y = (1 - utilisation) * availableHeight;
        double x = (i - prevBarCount) * barWidth;
        QRectF rect(x, y, barWidth, barWidth);
        rects.append(rect);
    }

    if (rects.isEmpty())
        return;

    QPen linePen(seriesColor, 3.0);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity(cpuSeriesOpacity_);
    painter.setPen(defaultEllipsePen_);
    painter.setBrush(seriesColor);

    if (rects.size() == 1) {
        auto rect = rects.first();
        painter.drawEllipse(rect);
        rectHovered(eventSeries->getKind(), rect);
    } else {
        for (int i = 1; i < rects.count(); i++) {
            auto rect1 = rects.at(i - 1);
            auto rect2 = rects.at(i);
            painter.setPen(linePen);
            painter.drawLine(rect1.center(), rect2.center());
            painter.setPen(defaultEllipsePen_);
            painter.drawEllipse(rect1);
            painter.drawEllipse(rect2);
            rectHovered(eventSeries->getKind(), rect1);
            rectHovered(eventSeries->getKind(), rect2);
        }
    }

    painter.setRenderHint(QPainter::Antialiasing, false);

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "CPU Render Took: " << finish - start << "ms";
}


/**
 * @brief Chart::paintMemoryUtilisationEventSeries
 * @param painter
 */
void Chart::paintMemoryUtilisationEventSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    const auto eventSeries = seriesList_.value(ChartDataKind::MEMORY_UTILISATION, nullptr);
    if (!eventSeries)
        return;

    const auto& events = eventSeries->getEvents();
    if  (events.isEmpty())
        return;

    double barWidth = POINT_WIDTH;
    double barCount = ceil((double)width() / barWidth);
    double barTimeWidth = (displayMax_ - displayMin_) / barCount;

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    // get the iterators to the left and right of the display range
    auto firstEventItr = std::lower_bound(events.constBegin(), events.constEnd(), displayMin_, [](const Event* e, const qint64 &time) {
        return e->getTimeMS() < time;
    });
    auto lastEventItr = std::upper_bound(events.constBegin(), events.constEnd(), displayMax_, [](const qint64 &time, const Event* e) {
        return time < e->getTimeMS();
    });

    // if the first iterator is the same as the last iterator, it means that all the events are out of range; return
    if (firstEventItr == lastEventItr) {
        return;
    }

    if (firstEventItr != events.constBegin()) {
        firstEventItr -= 1;
    }

    auto prevBarCount = 0.0;
    auto firstEventTime = (*firstEventItr)->getTimeMS();
    bool firstItrIsOutOfRange = (firstEventTime < displayMin_);
    if (firstItrIsOutOfRange) {
        prevBarCount = ceil((displayMin_ - firstEventTime) / barTimeWidth);
    }

    // get the iterator of the leftmost event up to the first bin that contributes to drawing
    auto first_contributing_event = firstEventItr;
    auto firstEndTime = displayMin_ - prevBarCount * barTimeWidth;
    if (firstItrIsOutOfRange) {
        for (; first_contributing_event != events.constBegin(); --first_contributing_event) {
            const auto& current_time = (*first_contributing_event)->getTimeMS();
            // keep going until we overshoot, then move back if we can
            if (current_time < firstEndTime) {
                first_contributing_event++;
                break;
            }
        }
    }

    auto postBarCount = 0.0;
    bool lastItrIsOutOfRange = false;
    if (lastEventItr != events.constEnd()) {
        auto lastEventTime = (*lastEventItr)->getTimeMS();
        lastItrIsOutOfRange = (lastEventTime >= displayMax_);
        if (lastItrIsOutOfRange) {
            postBarCount = ceil((lastEventTime - displayMax_) / barTimeWidth);
        }
    }

    // get the iterator of the rightmost event up to the last bin that contributes to drawing
    auto last_contributing_event = lastEventItr;
    if (lastItrIsOutOfRange) {
        auto lastEndTime = displayMax_ + postBarCount * barTimeWidth;
        for (; last_contributing_event != events.constEnd(); ++last_contributing_event) {
            const auto& current_time = (*last_contributing_event)->getTimeMS();
            // keep going until we overshoot, then move back if we can
            if (current_time >= lastEndTime) {
                if (last_contributing_event != events.constBegin()) {
                    last_contributing_event--;
                    break;
                }
            }
        }
    }

    auto totalBarCount = prevBarCount + barCount + postBarCount;
    auto currentLeft = firstEndTime;

    QVector< QList<MemoryUtilisationEvent*> > buckets(totalBarCount);
    QVector<double> bucketEndTimes;
    bucketEndTimes.reserve(totalBarCount);

    // calculate the bucket end times
    for (int i = 0; i < totalBarCount; i++) {
        bucketEndTimes.append(currentLeft + barTimeWidth);
        currentLeft = bucketEndTimes.last();
    }

    auto currentBucketIndex = 0;
    auto currentBucketItr = bucketEndTimes.constBegin();
    auto endBucketItr = bucketEndTimes.constEnd();

    // put the data in the correct bucket
    while (first_contributing_event != events.constEnd()) {
        auto event = (MemoryUtilisationEvent*)(*first_contributing_event);
        const auto& currentTime = event->getTimeMS();
        while (currentBucketItr != endBucketItr) {
            if (currentTime >= (*currentBucketItr)) {
                currentBucketItr++;
                currentBucketIndex++;
            } else {
                break;
            }
        }
        if (currentBucketIndex < totalBarCount) {
            buckets[currentBucketIndex].append(event);
        }
        if (first_contributing_event == last_contributing_event) {
            break;
        }
        first_contributing_event++;
    }

    auto availableHeight = height() - barWidth;
    auto seriesColor = memoryColor_;
    QList<QRectF> rects;

    for (int i = 0; i < totalBarCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;

        // calculate the bucket's average utilisation
        auto utilisation = 0.0;
        for (auto e : buckets[i]) {
            utilisation += e->getUtilisation();
        }
        utilisation /= count;

        double y = (1 - utilisation) * availableHeight;
        double x = (i - prevBarCount) * barWidth;
        QRectF rect(x, y, barWidth, barWidth);
        rects.append(rect);
    }

    if (rects.isEmpty())
        return;

    QPen linePen(seriesColor, 3.0);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity(memorySeriesOpacity_);
    painter.setPen(defaultEllipsePen_);
    painter.setBrush(seriesColor);

    if (rects.size() == 1) {
        auto rect = rects.first();
        painter.drawEllipse(rect);
        rectHovered(eventSeries->getKind(), rect);
    } else {
        for (int i = 1; i < rects.count(); i++) {
            auto rect1 = rects.at(i - 1);
            auto rect2 = rects.at(i);
            painter.setPen(linePen);
            painter.drawLine(rect1.center(), rect2.center());
            painter.setPen(defaultEllipsePen_);
            painter.drawEllipse(rect1);
            painter.drawEllipse(rect2);
            rectHovered(eventSeries->getKind(), rect1);
            rectHovered(eventSeries->getKind(), rect2);
        }
    }

    painter.setRenderHint(QPainter::Antialiasing, false);

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "MEMORY Render Took: " << finish - start << "ms";
}


/**
 * @brief Chart::paintMarkerEventSeries
 * @param painter
 */
void Chart::paintMarkerEventSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    const auto eventSeries = seriesList_.value(ChartDataKind::MARKER, nullptr);
    if (!eventSeries)
        return;

    double binWidth = BIN_WIDTH;
    double binCount = ceil((double)width() / binWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    binWidth = (double) width() / binCount;

    QVector<double> bins(binCount);
    QVector<double> binEndTimes;
    binEndTimes.reserve(binCount);

    double binTimeWidth = (displayMax_ - displayMin_) / binCount;
    double currentLeft = displayMin_;
    for (int i = 0; i < binCount; i++) {
        binEndTimes.append(currentLeft + binTimeWidth);
        currentLeft = binEndTimes.last();
    }

    auto markerEventSeries = (MarkerEventSeries*) eventSeries;
    const auto& startTimesMap = markerEventSeries->getMarkerIDsMappedByStartTimes();
    const auto& idSetDuration = markerEventSeries->getMarkerIDSetDurations();

    auto currentBinIndex = 0;
    auto currentBinItr = binEndTimes.constBegin();
    const auto endBinItr = binEndTimes.constEnd();
    const auto& startTimes = startTimesMap.keys();
    auto startTimeItr = startTimes.constBegin();
    const auto endStartTimeItr = startTimes.constEnd();

    while (currentBinItr != endBinItr) {
        auto currentBinEndTime = *currentBinItr;
        auto totalDuration = 0.0;
        auto numberOfIDSets = 0;
        for (; startTimeItr != endStartTimeItr; startTimeItr++) {
            const auto& startTime = *startTimeItr;
            // skip start times that are out of the display range
            if (startTime < displayMin_) {
                continue;
            }
            if (startTime >= displayMax_) {
                break;
            }
            if (startTime > currentBinEndTime) {
                break;
            }
            // calculate average duration
            const auto& IDSetsAtStartTime = startTimesMap.value(startTime);
            for (auto ID : IDSetsAtStartTime) {
                totalDuration += idSetDuration.value(ID);
                numberOfIDSets++;
            }
        }

        // store the average duration at the current bin
        auto avgDuration = 0.0;
        if (numberOfIDSets > 0) {
            avgDuration = (totalDuration == 0) ? -1 : totalDuration / numberOfIDSets;
        }
        bins[currentBinIndex] = avgDuration;

        //bins[currentBinIndex] = (numberOfIDSets == 0) ? 0 : totalDuration / numberOfIDSets;
        currentBinItr++;
        currentBinIndex++;

        if (currentBinIndex >= binCount)
            break;
    }

    // get the maximum duration
    auto maxDuration = 0.0;
    for (int i = 0; i < bins.count(); i++) {
        maxDuration = qMax(bins[i], maxDuration);
    }

    QColor seriesColor = markerColor_;
    QColor brushColor = seriesColor;
    QPen textPen(textColor_, 2.0);

    auto availableHeight = height() - BORDER_WIDTH;
    painter.setOpacity(markerSeriesOpacity_);

    for (int i = 0; i < binCount; i++) {
        auto durationMS = bins[i];
        if (durationMS == 0) {
            continue;
        }
        auto rectHeight = (maxDuration <= 0) ? availableHeight : durationMS / maxDuration * availableHeight;
        if (durationMS == -1) {
            rectHeight = 2.0;
        }
        auto y = availableHeight - rectHeight + BORDER_WIDTH / 2.0;
        QRectF rect(i * binWidth, y, binWidth, rectHeight);
        if (rectHovered(eventSeries->getKind(), rect)) {
            textPen.setColor(textColor_);
            painter.fillRect(rect, highlightBrush_);
        } else {
            brushColor = (maxDuration <= 0) ? seriesColor.darker(150) : seriesColor.darker(100 + (50 * durationMS / maxDuration));
            textPen.setColor(getContrastingColor(brushColor));
            painter.setPen(defaultRectPen_);
            painter.setBrush(brushColor);
            painter.drawRect(rect);
        }
    }

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "MARKER Render Took: " << finish - start << "ms";
}


/**
 * @brief Chart::paintPortLifecycleSeries
 * @param painter
 */
void Chart::paintPortLifecycleSeries(QPainter &painter)
{
    if (portLifecycleBinnedData_.size() == 0)
        return;

    int firstIndex = getBinIndexForTime(displayMin_);
    int lastIndex = getBinIndexForTime(displayMax_)+1;

    QColor seriesColor = portLifecycleColor_;
    int y = rect().center().y() - BIN_WIDTH / 2.0;

    for (int i = firstIndex; (i <= lastIndex) && (i < portLifecycleBinnedData_.size()); i++) {
        int count = portLifecycleBinnedData_[i].count();
        if (count == 0)
            continue;
        auto rectX = (i - firstIndex) * binPixelWidth_;
        QRectF rect(rectX, y, BIN_WIDTH, BIN_WIDTH);
        qDebug() << "i[" << i << "] - rect x: " << rect.x();
        if (count == 1) {
            auto event = (PortLifecycleEvent*) portLifecycleBinnedData_[i][0];
            if (rectHovered(ChartDataKind::PORT_LIFECYCLE, rect))
                painter.fillRect(rect, highlightColor_);
            painter.drawPixmap(rect.toRect(), lifeCycleTypePixmaps_.value(event->getType()));
        } else {
            QColor color = seriesColor.darker(100 + (50 * (count - 1)));
            painter.setPen(Qt::lightGray);
            if (rectHovered(ChartDataKind::PORT_LIFECYCLE, rect)) {
                painter.setPen(highlightTextColor_);
                color = highlightColor_;
            }
            painter.fillRect(rect, color);
            painter.drawText(rect, QString::number(count), QTextOption(Qt::AlignCenter));
        }
    }
    qDebug() << "-------------------";
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
        // if the hit rect is hovered, store/update the hovered time range for the provided series kind
        QPair<qint64, qint64> timeRange = {mapPixelToTime(hitRect.left()), mapPixelToTime(hitRect.right())};
        if (hoveredSeriesTimeRange_.contains(kind)) {
            timeRange.first = hoveredSeriesTimeRange_.value(kind).first;
        }
        hoveredSeriesTimeRange_[kind] = timeRange;
        if (kind == ChartDataKind::CPU_UTILISATION || kind == ChartDataKind::MEMORY_UTILISATION) {
            hoveredEllipseRects_.append(hitRect);
        } else { //if (kind != ChartDataKind::MARKER) {
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
    if (!seriesList_.contains(kind))
        return;

    auto dataRange = dataMaxX_ - dataMinX_;
    auto displayRange = displayMax_ - displayMin_;

    auto binRatio =  BIN_WIDTH / (double) width();
    binTimeWidth_ = binRatio * displayRange;
    binCount_ = ceil(dataRange / binTimeWidth_);
    binPixelWidth_ = (double) width() / binCount_;
    //binPixelWidth_ = BIN_WIDTH;
    //binPixelWidth_ = (double)width() / (dataRange / binTimeWidth_);

    //binRatio = binPixelWidth_ / (double) width();
    //binTimeWidth_ = binRatio * displayRange;
    //binCount_ = /*ceil*/(dataRange / binTimeWidth_);

    if (kind == ChartDataKind::PORT_LIFECYCLE) {
        int firstIndex = getBinIndexForTime(displayMin_);
        int lastIndex = getBinIndexForTime(displayMax_);
        qDebug() << "width(): " << width();
        qDebug() << "binRatio =" << binRatio;
        qDebug() << "bin pixel width: " << binPixelWidth_;
        qDebug() << "bin time width: " << binTimeWidth_;
        qDebug() << "bins#: " << binCount_;
        qDebug() << "first index: " << firstIndex << " - 0";
        qDebug() << "last index: " << lastIndex << " - " << ((lastIndex - firstIndex) * binPixelWidth_);
    }

    // clear binned data
    auto& binnedData = getBinnedData(kind);
    binnedData.clear();
    binnedData.resize(binCount_);

    //qDebug() << "Update binned data: " << QDateTime::fromMSecsSinceEpoch(dataMinX_).toString(TIME_FORMAT) << ", " << QDateTime::fromMSecsSinceEpoch(dataMaxX_).toString(TIME_FORMAT);

    QVector<double> binEndTimes;
    binEndTimes.reserve(binCount_);

    // calculate the bin end times for the whole data range
    auto currentTime = dataMinX_;
    for (auto i = 0; i < binCount_; i++) {
        /*if (kind == ChartDataKind::PORT_LIFECYCLE)
            qDebug() << "CURRENT time: " << QDateTime::fromMSecsSinceEpoch(currentTime).toString(TIME_FORMAT);*/
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
                if (kind == ChartDataKind::PORT_LIFECYCLE)
                    qDebug() << "--- bin data at: " << currentBin;
                binnedData[currentBin].append(*eventItr);
            }
        }
    }
}


/**
 * @brief Chart::updateSeriesPixmaps
 */
void Chart::updateSeriesPixmaps()
{
    Theme* theme = Theme::theme();
    bool colorPortPixmaps = false;
    bool colorWorkerPixmaps = false;

    switch (hoveredSeriesKind_) {
    case ChartDataKind::DATA: {
        colorPortPixmaps = true;
        colorWorkerPixmaps = true;
        break;
    }
    case ChartDataKind::PORT_LIFECYCLE:
        colorPortPixmaps = true;
        break;
    case ChartDataKind::WORKLOAD:
        colorWorkerPixmaps = true;
        break;
    default:
        break;
    }

    if (colorPortPixmaps) {
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::NO_TYPE] = theme->getImage("Icons", "circleQuestion", QSize(), theme->getAltTextColor());
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::CONFIGURE] = theme->getImage("Icons", "gear", QSize(), QColor(179, 204, 230)); //theme->getMenuIconColor());
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::ACTIVATE] = theme->getImage("Icons", "power", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::PASSIVATE] = theme->getImage("Icons", "bed", QSize(), QColor(255, 179, 102)); //theme->getSeverityColor(Notification::Severity::ERROR));
        lifeCycleTypePixmaps_[AggServerResponse::LifecycleType::TERMINATE] = theme->getImage("Icons", "cancel", QSize(), theme->getSeverityColor(Notification::Severity::ERROR)); //theme->getMenuIconColor());
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
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleMessage", QSize(), QColor(72, 151, 189));
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::WARNING] = theme->getImage("Icons", "triangleCritical", QSize(), theme->getSeverityColor(Notification::Severity::WARNING));
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::ERROR_EVENT] = theme->getImage("Icons", "circleCrossDark", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MARKER] = theme->getImage("Icons", "bookmarkTwoTone", QSize(), QColor(72, 151, 199));
    } else {
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::STARTED] = theme->getImage("Icons", "play", QSize(), backgroundColor_);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::FINISHED] = theme->getImage("Icons", "avStop", QSize(), backgroundColor_);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleMessage", QSize(), backgroundColor_);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::WARNING] = theme->getImage("Icons", "triangleCritical", QSize(), backgroundColor_);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::ERROR_EVENT] = theme->getImage("Icons", "circleCrossDark", QSize(), backgroundColor_);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MARKER] = theme->getImage("Icons", "bookmarkTwoTone", QSize(), backgroundColor_);
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
 * @brief Chart::getBinIndexForTime
 * @param time
 * @return
 */
int Chart::getBinIndexForTime(double time)
{
    auto index = (time - dataMinX_) / binTimeWidth_;
    //qDebug() << "index: " << index << " @ time " << (time - dataMinX_);
    return floor(index);
}


/**
 * @brief Chart::getBinnedData
 * @param kind
 * @return
 */
QVector<QList<Event*>> &Chart::getBinnedData(ChartDataKind kind)
{
    switch (kind) {
    case ChartDataKind::PORT_LIFECYCLE:
        return portLifecycleBinnedData_;
    case ChartDataKind::WORKLOAD:
        return workloadBinnedData_;
    case ChartDataKind::CPU_UTILISATION:
        return cpuUtilisationBinnedData_;
    case ChartDataKind::MEMORY_UTILISATION:
        return memoryUtilisationBinnedData_;
    case ChartDataKind::MARKER:
        return markerBinnedData_;
    default:
        return emptyBinnedData_;
    }
}


/**
 * @brief Chart::mapPixelToTime
 * @param x
 * @return
 */
qint64 Chart::mapPixelToTime(double x)
{
    auto timeRange = displayMax_ - displayMin_;
    auto ratio = x / width();
    return displayMin_ + timeRange * ratio;
}


/**
 * @brief Chart::mapTimeToPixel
 * @param time
 * @return
 */
double Chart::mapTimeToPixel(double time)
{
    auto timeRange = displayMax_ - displayMin_;
    auto adjustedTime = time - displayMin_;
    return adjustedTime / timeRange * width();
}


/**
 * @brief qHash
 * @param key
 * @param seed
 * @return
 */
inline uint qHash(ChartDataKind key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}



