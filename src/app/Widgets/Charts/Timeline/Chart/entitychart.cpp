#include "entitychart.h"
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
#define BIN_WIDTH 24.0
#define POINT_WIDTH 8.0

#define PRINT_RENDER_TIMES false


/**
 * @brief EntityChart::EntityChart
 * @param experimentRunID
 * @param experimentStartTime
 * @param parent
 */
EntityChart::EntityChart(quint32 experimentRunID, qint64 experimentStartTime, QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    experimentRunID_ = experimentRunID;
    experimentRunStartTime_ = experimentStartTime;
    hoveredSeriesKind_ = TIMELINE_DATA_KIND::DATA;

    dataMinX_ = DBL_MAX;
    dataMaxX_ = DBL_MIN;
    dataMinY_ = DBL_MAX;
    dataMaxY_ = DBL_MIN;

    /*
    //_color_s_state = 35;
    _color_s_state = 50;
    _color_s_notification = 100;
    _color_s_line = 200;
    */

    connect(Theme::theme(), &Theme::theme_Changed, this, &EntityChart::themeChanged);
    themeChanged();
}


/**
 * @brief EntityChart::getExperimentRunID
 * @return
 */
quint32 EntityChart::getExperimentRunID()
{
    return experimentRunID_;
}


/**
 * @brief EntityChart::addSeries
 * @param series
 */
void EntityChart::addSeries(MEDEA::EventSeries* series)
{
    if (series) {
        seriesList_[series->getKind()] = series;
        if (series->getKind() == TIMELINE_DATA_KIND::CPU_UTILISATION ||
            series->getKind() == TIMELINE_DATA_KIND::MEMORY_UTILISATION) {
            containsYRange_ = true;
        }
        connect(series, &MEDEA::EventSeries::minYValueChanged, this, &EntityChart::updateVerticalMin);
        connect(series, &MEDEA::EventSeries::maxYValueChanged, this, &EntityChart::updateVerticalMax);
    }
}


/**
 * @brief EntityChart::removeSeries
 * @param kind
 */
void EntityChart::removeSeries(TIMELINE_DATA_KIND kind)
{
    seriesList_.remove(kind);
}


/**
 * @brief EntityChart::getSeries
 * @return
 */
const QHash<TIMELINE_DATA_KIND, MEDEA::EventSeries*>& EntityChart::getSeries()
{
    return seriesList_;
}


/**
 * @brief EntityChart::getHovereSeriesKinds
 * @return
 */
const QList<TIMELINE_DATA_KIND> EntityChart::getHovereSeriesKinds()
{
    return hoveredSeriesTimeRange_.keys();
}


/**
 * @brief EntityChart::getHoveredTimeRange
 * @param kind
 * @return
 */
const QPair<qint64, qint64> EntityChart::getHoveredTimeRange(TIMELINE_DATA_KIND kind)
{
    return hoveredSeriesTimeRange_.value(kind, {-1, -1});
}


/**
 * @brief EntityChart::setRange
 * @param min
 * @param max
 */
void EntityChart::setRange(double min, double max)
{
    // add 1% on either side to include border values
    auto range = max - min;
    min = min - (range * 0.01);
    max = max + (range * 0.01);

    dataMinX_ = min;
    dataMaxX_ = max;

    displayMin_ = (dataMaxX_ - dataMinX_) * minRatio_ + dataMinX_;
    displayMax_ = (dataMaxX_ - dataMinX_) * maxRatio_ + dataMinX_;

    //qDebug() << "Set RANGES to: " << QDateTime::fromMSecsSinceEpoch(min).toString(TIME_FORMAT) << ", " << QDateTime::fromMSecsSinceEpoch(max).toString(TIME_FORMAT);

    update();
}


/**
 * @brief EntityChart::setDisplayMinRatio
 * @param ratio
 */
void EntityChart::setDisplayMinRatio(double ratio)
{
    minRatio_ = ratio;
    displayMin_ = (dataMaxX_ - dataMinX_) * ratio + dataMinX_;
    update();
}


/**
 * @brief EntityChart::setDisplayMaxRatio
 * @param ratio
 */
void EntityChart::setDisplayMaxRatio(double ratio)
{
    maxRatio_ = ratio;
    displayMax_ = (dataMaxX_ - dataMinX_) * ratio + dataMinX_;
    update();
}


/**
 * @brief EntityChart::setDisplayRangeRatio
 * @param minRatio
 * @param maxRatio
 */
void EntityChart::setDisplayRangeRatio(double minRatio, double maxRatio)
{
    minRatio_ = minRatio;
    maxRatio_ = maxRatio;
    displayMin_ = (dataMaxX_ - dataMinX_) * minRatio + dataMinX_;
    displayMax_ = (dataMaxX_ - dataMinX_) * maxRatio + dataMinX_;
    update();
}


/**
 * @brief EntityChart::updateRange
 * @param startTime
 * @param duration
 */
void EntityChart::updateRange(double startTime, double duration)
{
    if (startTime == 0.0) {
        startTime = experimentRunStartTime_;
    }
    setRange(startTime, startTime + duration);
    setDisplayRangeRatio(0.0, 1.0);
    updateBinnedData();
}


/**
 * @brief EntityChart::updateBinnedData
 * @param kinds
 */
void EntityChart::updateBinnedData(QSet<TIMELINE_DATA_KIND> kinds)
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
 * @brief EntityChart::updateVerticalMin
 * @param min
 */
void EntityChart::updateVerticalMin(double min)
{
    dataMinY_ = min;
    update();
}


/**
 * @brief EntityChart::updateChartHeight
 * @param max
 */
void EntityChart::updateVerticalMax(double max)
{
    dataMaxY_ = max;
    update();
}


/**
 * @brief EntityChart::isHovered
 * @return
 */
bool EntityChart::isHovered()
{
    return hovered_;
}


/**
 * @brief EntityChart::setHovered
 * @param visible
 */
void EntityChart::setHovered(bool visible)
{
    hovered_ = visible;
}


/**
 * @brief EntityChart::setHoveredRect
 * @param rect
 */
void EntityChart::setHoveredRect(QRectF rect)
{
    if (rect != hoveredRect_) {
        QPoint pos = mapFromParent(rect.topLeft().toPoint());
        rect.moveTo(pos.x(), 0);
        hoveredRect_ = rect;
    }
}


/**
 * @brief EntityChart::setSeriesKindVisible
 * @param kind
 * @param visible
 */
void EntityChart::setSeriesKindVisible(TIMELINE_DATA_KIND kind, bool visible)
{
    seriesKindVisible_[kind] = visible;
    update();
}


/**
 * @brief EntityChart::seriesKindHovered
 * @param kind
 */
void EntityChart::seriesKindHovered(TIMELINE_DATA_KIND kind)
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
        break;
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE: {
        portLifecycleColor_ = defaultPortLifecycleColor_;
        portSeriesOpacity_ = 1.0;
        break;
    }
    case TIMELINE_DATA_KIND::WORKLOAD: {
        workloadColor_ = defaultWorkloadColor_;
        workloadSeriesOpacity_ = 1.0;
        break;
    }
    case TIMELINE_DATA_KIND::CPU_UTILISATION: {
        utilisationColor_ = defaultUtilisationColor_;
        cpuSeriesOpacity_ = 1.0;
        break;
    }
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION: {
        memoryColor_ = defaultMemoryColor_;
        memorySeriesOpacity_ = 1.0;
        break;
    }
    case TIMELINE_DATA_KIND::MARKER: {
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
 * @brief EntityChart::themeChanged
 */
void EntityChart::themeChanged()
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

    textColor_ = theme->getTextColor();
    gridColor_ = theme->getAltTextColor();
    highlightTextColor_ = theme->getTextColor(ColorRole::SELECTED);
    backgroundColor_ = theme->getAltBackgroundColor();
    backgroundColor_.setAlphaF(BACKGROUND_OPACITY);
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
 * @brief EntityChart::resizeEvent
 * @param event
 */
void EntityChart::resizeEvent(QResizeEvent* event)
{
    updateBinnedData();
    update();
    QWidget::resizeEvent(event);
}


/**
 * @brief EntityChart::paintEvent
 * @param event
 */
void EntityChart::paintEvent(QPaintEvent* event)
{
    /*
     * NOTE - This assumes that the data is ordered in ascending order time-wise
     */

    auto start = QDateTime::currentMSecsSinceEpoch();
    const auto& paintOrder = GET_TIMELINE_DATA_KINDS();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, false);

    clearHoveredLists();

    for (const auto& kind : paintOrder) {
        if (kind != hoveredSeriesKind_)
            paintSeries(painter, kind);
    }
    paintSeries(painter, hoveredSeriesKind_);
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
            QRectF maxRect(width() - w, BORDER_WIDTH, w, h);
            QRectF minRect(width() - w, height() - h - BORDER_WIDTH, w, h);
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
    if (PRINT_RENDER_TIMES)
        qDebug() << "Total Series Render Took: " << finish - start << "MS.";
}


/**
 * @brief EntityChart::paintSeries
 * @param painter
 * @param kind
 */
void EntityChart::paintSeries(QPainter &painter, TIMELINE_DATA_KIND kind)
{
    if (!seriesKindVisible_.value(kind, false))
        return;

    // draw the points and get intersection point(s) index
    switch (kind) {
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
        paintPortLifecycleEventSeries(painter);
        break;
    case TIMELINE_DATA_KIND::WORKLOAD:
        paintWorkloadEventSeries(painter);
        break;
    case TIMELINE_DATA_KIND::CPU_UTILISATION:
        paintCPUUtilisationEventSeries(painter);
        break;
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
        paintMemoryUtilisationEventSeries(painter);
        break;
    case TIMELINE_DATA_KIND::MARKER:
        paintMarkerEventSeries(painter);
        break;
    default:
        qWarning("EntityChart::paintSeries - Series kind not handled");
        break;
    }
}


/**
 * @brief EntityChart::paintPortLifecycleEventSeries
 * @param painter
 */
void EntityChart::paintPortLifecycleEventSeries(QPainter &painter)
{
    MEDEA::EventSeries* eventSeries = seriesList_.value(TIMELINE_DATA_KIND::PORT_LIFECYCLE, 0);
    if (!eventSeries)
        return;

    double barWidth = BIN_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<MEDEA::Event*> > buckets(barCount);
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
                /*
                 *  TODO - This forces the hover display to only show the hovered item's data/time
                 *  This can be removed when the date-time axis range has a minimum limit
                 *  This also needs to be changed when there are multiple series of the same kind
                 */
                hoveredSeriesTimeRange_[eventSeries->getKind()] = {event->getTimeMS(), event->getTimeMS()};
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
}


/**
 * @brief EntityChart::paintWorkloadEventSeries
 * @param painter
 */
void EntityChart::paintWorkloadEventSeries(QPainter &painter)
{
    MEDEA::EventSeries* eventSeries = seriesList_.value(TIMELINE_DATA_KIND::WORKLOAD, 0);
    if (!eventSeries)
        return;

    double barWidth = BIN_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<MEDEA::Event*> > buckets(barCount);
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
                /*
                 *  TODO - This forces the hover display to only show the hovered item's data/time
                 *  This can be removed when the date-time axis range has a minimum limit
                 *  This also needs to be changed when there are multiple series of the same kind
                 */
                hoveredSeriesTimeRange_[eventSeries->getKind()] = {event->getTimeMS(), event->getTimeMS()};
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
}


/**
 * @brief EntityChart::paintCPUUtilisationEventSeries
 * @param painter
 */
void EntityChart::paintCPUUtilisationEventSeries(QPainter &painter)
{
    MEDEA::EventSeries* eventSeries = seriesList_.value(TIMELINE_DATA_KIND::CPU_UTILISATION, 0);
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

     auto firstEventItr = events.constBegin();
     auto lastEventItr = events.constEnd();
     auto firstItrSet = false;

     // get the event iterators to the left and right of the display range
     for (auto current = events.constBegin(); current != events.constEnd(); current++) {
         const auto& current_time = (*current)->getTimeMS();
         if (!firstItrSet && (current_time > displayMin_)) {
             firstEventItr = current == events.constBegin() ? current : current - 1;
             firstItrSet = true;
         }
         if (current_time >= displayMax_) {
             lastEventItr = current;
             break;
         }
     }

     auto prevBarCount = 0.0;
     if (firstEventItr != events.constEnd()) {
         prevBarCount = ceil((displayMin_ - ((CPUUtilisationEvent*)(*firstEventItr))->getTimeMS()) / barTimeWidth);
     }
     auto postBarCount = 0.0;
     if (lastEventItr != events.constEnd()) {
         postBarCount = ceil((((CPUUtilisationEvent*)(*lastEventItr))->getTimeMS() - displayMax_) / barTimeWidth);
     }

     // get the iterator of the leftmost event up to the first bin that contributes to drawing
     auto firstEndTime = displayMin_ - prevBarCount * barTimeWidth;
     auto first_contributing_event = firstEventItr;
     while (first_contributing_event != events.constBegin()) {
         const auto& current_time = (*first_contributing_event)->getTimeMS();
         // Keep going until we overshoot, then move back if we can
         if (current_time < firstEndTime) {
             if (first_contributing_event != events.constEnd()) {
                first_contributing_event++;
                break;
             }
         }
         first_contributing_event--;
     }

     // get the iterator of the rightmost event up to the last bin that contributes to drawing
     auto lastEndTime = displayMax_ + postBarCount * barTimeWidth;
     auto last_contributing_event = lastEventItr;
     while (last_contributing_event != events.constEnd()) {
         const auto& current_time = (*last_contributing_event)->getTimeMS();
         // keep going until we overshoot, then move back if we can
         if (current_time >= lastEndTime) {
             if (last_contributing_event != events.constBegin()) {
                last_contributing_event--;
                break;
             }
         }
         last_contributing_event++;
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
}


/**
 * @brief EntityChart::paintMemoryUtilisationEventSeries
 * @param painter
 */
void EntityChart::paintMemoryUtilisationEventSeries(QPainter &painter)
{
    MEDEA::EventSeries* eventSeries = seriesList_.value(TIMELINE_DATA_KIND::MEMORY_UTILISATION, 0);
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

     auto firstEventItr = events.constBegin();
     auto lastEventItr = events.constEnd();
     auto firstItrSet = false;

    // get the event iterators to the left and right of the display range
     for (auto current = events.constBegin(); current != events.constEnd(); current++) {
         const auto& current_time = (*current)->getTimeMS();
         if (!firstItrSet && (current_time > displayMin_)) {
             firstEventItr = current == events.constBegin() ? current : current - 1;
             firstItrSet = true;
         }
         if (current_time >= displayMax_) {
             lastEventItr = current;
             break;
         }
     }

     auto prevBarCount = 0.0;
     if (firstEventItr != events.constEnd()) {
         prevBarCount = ceil((displayMin_ - ((MemoryUtilisationEvent*)(*firstEventItr))->getTimeMS()) / barTimeWidth);
     }
     auto postBarCount = 0.0;
     if (lastEventItr != events.constEnd()) {
         postBarCount = ceil((((MemoryUtilisationEvent*)(*lastEventItr))->getTimeMS() - displayMax_) / barTimeWidth);
     }

     // get the iterator of the leftmost event up to the first bin that contributes to drawing
     auto firstEndTime = displayMin_ - prevBarCount * barTimeWidth;
     auto first_contributing_event = firstEventItr;
     while (first_contributing_event != events.constBegin()) {
         const auto& current_time = (*first_contributing_event)->getTimeMS();
         // keep going until we overshoot, then move back if we can
         if (current_time < firstEndTime) {
             if (first_contributing_event != events.constEnd()) {
                first_contributing_event++;
                break;
             }
         }
         first_contributing_event--;
     }

     // get the iterator of the rightmost event up to the last bin that contributes to drawing
     auto lastEndTime = displayMax_ + postBarCount * barTimeWidth;
     auto last_contributing_event = lastEventItr;
     while (last_contributing_event != events.constEnd()) {
         const auto& current_time = (*last_contributing_event)->getTimeMS();
         // Keep going until we overshoot, then move back if we can
         if (current_time >= lastEndTime) {
             if (last_contributing_event != events.constBegin()) {
                last_contributing_event--;
                break;
             }
         }
         last_contributing_event++;
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
}


/**
 * @brief EntityChart::paintMarkerEventSeries
 * @param painter
 */
void EntityChart::paintMarkerEventSeries(QPainter &painter)
{
    MEDEA::EventSeries* eventSeries = seriesList_.value(TIMELINE_DATA_KIND::MARKER, 0);
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
    const auto& startTimes = markerEventSeries->getMarkerIDsWithSharedStartTimes();
    const auto& idSetDuration = markerEventSeries->getMarkerIDSetDurations();

    auto currentBinIndex = 0;
    auto currentBinItr = binEndTimes.constBegin();
    auto endBinItr = binEndTimes.constEnd();

    for (const auto& startTime : startTimes.keys()) {
        if (startTime < displayMin_) {
            continue;
        }
        // find the right bin to put the data in
        while (currentBinItr != endBinItr) {
            if (startTime > (*currentBinItr)) {
                currentBinItr++;
                currentBinIndex++;
            } else {
                //qDebug() << "startime < bin time - index: " << currentBinIndex;
                break;
            }
        }
        // bin the data
        if (currentBinIndex < binCount) {
            const auto& markerIDsAtStartTime = startTimes.value(startTime);
            //qDebug() << "IDs#: " << markerIDsAtStartTime.count();
            auto totalDuration = 0.0;
            for (const auto& id : markerIDsAtStartTime) {
                if (idSetDuration.contains(id)) {
                    totalDuration += idSetDuration.value(id);
                    auto d = idSetDuration.value(id);
                    if (d > 0) {
                        qDebug() << "id: " << id;
                        qDebug() << "IDs#: " << markerIDsAtStartTime.count();
                        qDebug() << "duration: " << d;
                        qDebug() << "totalDuration: " << totalDuration;
                        qDebug()  << "BIN DATA --- " << QString::number(totalDuration / markerIDsAtStartTime.count());
                        qDebug() << "at: " << currentBinIndex;
                        qDebug() << "------";
                    }
                }
            }
            // bin the average duration from the current start time
            bins[currentBinIndex] = totalDuration / markerIDsAtStartTime.count();
        }
    }

    // get the maximum duration
    auto maxDuration = 0.0;
    for (int i = 0; i < bins.count(); i++) {
        auto binVal = bins[i];
        qDebug() << "binVal: " << binVal;
        //maxDuration = qMax(binVal, maxDuration);
    }

    qDebug() << "maxDuration: " << maxDuration;

    QColor seriesColor = markerColor_;
    QColor brushColor = seriesColor;
    QPen textPen(textColor_, 2.0);

    auto availableHeight = height() - BORDER_WIDTH;
    painter.setOpacity(markerSeriesOpacity_);

    for (int i = 0; i < binCount; i++) {
        auto durationMS = bins[i];
        if (durationMS == 0)
            continue;
        qDebug() << "duration: " << durationMS;
        double rectHeight = (maxDuration <= 0) ? availableHeight : durationMS / maxDuration * availableHeight; // + BORDER_WIDTH / 2.0;
        double y = availableHeight - rectHeight + BORDER_WIDTH / 2.0;
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
        //painter.drawRect(rect);

        //QString countStr = count > 99 ? "🤮" : QString::number(count);
        //painter.drawText(rect, countStr, QTextOption(Qt::AlignCenter));
        //QString countStr = durationMS > 99 ? "𝑛" : QString::number(durationMS);
        //painter.drawText(rect, QString::number(durationMS) + "ms", QTextOption(Qt::AlignCenter));
        /*
        painter.setPen(textPen);
        painter.translate(0, availableHeight);
        painter.rotate(-90);
        painter.drawText(rect.x(), rect.y(), rectHeight, rect.width(), Qt::AlignCenter, QString::number(durationMS) + "ms");
        painter.rotate(90);
        painter.translate(0, -availableHeight);
        */
    }
}


/**
 * @brief EntityChart::paintPortLifecycleSeries
 * @param painter
 */
void EntityChart::paintPortLifecycleSeries(QPainter &painter)
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
            if (rectHovered(TIMELINE_DATA_KIND::PORT_LIFECYCLE, rect))
                painter.fillRect(rect, highlightColor_);
            painter.drawPixmap(rect.toRect(), lifeCycleTypePixmaps_.value(event->getType()));
        } else {
            QColor color = seriesColor.darker(100 + (50 * (count - 1)));
            painter.setPen(Qt::lightGray);
            if (rectHovered(TIMELINE_DATA_KIND::PORT_LIFECYCLE, rect)) {
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
 * @brief EntityChart::rectHovered
 * @param kind
 * @param hitRect
 * @return
 */
bool EntityChart::rectHovered(TIMELINE_DATA_KIND kind, const QRectF& hitRect)
{
    auto painterRect = hitRect.adjusted(-PEN_WIDTH / 2.0, 0, PEN_WIDTH / 2.0, 0);
    if (rectHovered(painterRect)) {
        // if the hit rect is hovered, store/update the hovered time range for the provided series kind
        QPair<qint64, qint64> timeRange = {mapPixelToTime(hitRect.left()), mapPixelToTime(hitRect.right())};
        if (hoveredSeriesTimeRange_.contains(kind)) {
            timeRange.first = hoveredSeriesTimeRange_.value(kind).first;
        }
        hoveredSeriesTimeRange_[kind] = timeRange;
        if (kind == TIMELINE_DATA_KIND::CPU_UTILISATION || kind == TIMELINE_DATA_KIND::MEMORY_UTILISATION) {
            hoveredEllipseRects_.append(hitRect);
        } else {
            hoveredRects_.append(hitRect);
        }
        return true;
    }
    return false;
}


/**
 * @brief EntityChart::rectHovered
 * @param hitRect
 * @return
 */
bool EntityChart::rectHovered(const QRectF &hitRect)
{
    return hoveredRect_.intersects(hitRect);
}


/**
 * @brief EntityChart::clearHoveredLists
 */
void EntityChart::clearHoveredLists()
{
    // clear previously hovered series kinds
    hoveredSeriesTimeRange_.clear();
    hoveredEllipseRects_.clear();
    hoveredRects_.clear();
}


/**
 * @brief EntityChart::updateBinnedData
 * @param kind
 */
void EntityChart::updateBinnedData(TIMELINE_DATA_KIND kind)
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

    if (kind == TIMELINE_DATA_KIND::PORT_LIFECYCLE) {
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
        /*if (kind == TIMELINE_DATA_KIND::PORT_LIFECYCLE)
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
                if (kind == TIMELINE_DATA_KIND::PORT_LIFECYCLE)
                    qDebug() << "--- bin data at: " << currentBin;
                binnedData[currentBin].append(*eventItr);
            }
        }
    }
}


/**
 * @brief EntityChart::updateSeriesPixmaps
 */
void EntityChart::updateSeriesPixmaps()
{
    Theme* theme = Theme::theme();
    bool colorPortPixmaps = false;
    bool colorWorkerPixmaps = false;

    switch (hoveredSeriesKind_) {
    case TIMELINE_DATA_KIND::DATA: {
        colorPortPixmaps = true;
        colorWorkerPixmaps = true;
        break;
    }
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
        colorPortPixmaps = true;
        break;
    case TIMELINE_DATA_KIND::WORKLOAD:
        colorWorkerPixmaps = true;
        break;
    default:
        break;
    }

    if (colorPortPixmaps) {
        lifeCycleTypePixmaps_[LifecycleType::NO_TYPE] = theme->getImage("Icons", "circleQuestion", QSize(), theme->getAltTextColor());
        lifeCycleTypePixmaps_[LifecycleType::CONFIGURE] = theme->getImage("Icons", "gear", QSize(), theme->getSeverityColor(Notification::Severity::WARNING));
        lifeCycleTypePixmaps_[LifecycleType::ACTIVATE] = theme->getImage("Icons", "clockDark", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
        lifeCycleTypePixmaps_[LifecycleType::PASSIVATE] = theme->getImage("Icons", "circleMinusDark", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
        lifeCycleTypePixmaps_[LifecycleType::TERMINATE] = theme->getImage("Icons", "circleRadio", QSize(), theme->getMenuIconColor());
    } else {
        lifeCycleTypePixmaps_[LifecycleType::NO_TYPE] = theme->getImage("Icons", "circleQuestion", QSize(), backgroundColor_);
        lifeCycleTypePixmaps_[LifecycleType::CONFIGURE] = theme->getImage("Icons", "gear", QSize(), backgroundColor_);
        lifeCycleTypePixmaps_[LifecycleType::ACTIVATE] = theme->getImage("Icons", "clockDark", QSize(), backgroundColor_);
        lifeCycleTypePixmaps_[LifecycleType::PASSIVATE] = theme->getImage("Icons", "circleMinusDark", QSize(), backgroundColor_);
        lifeCycleTypePixmaps_[LifecycleType::TERMINATE] = theme->getImage("Icons", "circleRadio", QSize(), backgroundColor_);
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
 * @brief EntityChart::getContrastingColor
 * @param color
 * @return
 */
QColor EntityChart::getContrastingColor(const QColor &color)
{
    auto hue = color.hue();
    auto saturation = color.saturation() < 128 ? 255 : 0;
    auto value = color.value() < 128 ? 255 : 0;

    QColor contrastColor;
    contrastColor.setHsv(hue, saturation, value);
    return contrastColor;
}


/**
 * @brief EntityChart::getBinIndexForTime
 * @param time
 * @return
 */
int EntityChart::getBinIndexForTime(double time)
{
    auto index = (time - dataMinX_) / binTimeWidth_;
    //qDebug() << "index: " << index << " @ time " << (time - dataMinX_);
    return floor(index);
}


/**
 * @brief EntityChart::getBinnedData
 * @param kind
 * @return
 */
QVector<QList<MEDEA::Event*>> &EntityChart::getBinnedData(TIMELINE_DATA_KIND kind)
{
    switch (kind) {
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
        return portLifecycleBinnedData_;
    case TIMELINE_DATA_KIND::WORKLOAD:
        return workloadBinnedData_;
    case TIMELINE_DATA_KIND::CPU_UTILISATION:
        return cpuUtilisationBinnedData_;
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
        return memoryUtilisationBinnedData_;
    case TIMELINE_DATA_KIND::MARKER:
        return markerBinnedData_;
    default:
        return emptyBinnedData_;
    }
}


/**
 * @brief EntityChart::mapPixelToTime
 * @param x
 * @return
 */
qint64 EntityChart::mapPixelToTime(double x)
{
    auto timeRange = displayMax_ - displayMin_;
    auto ratio = x / width();
    return displayMin_ + timeRange * ratio;
}


/**
 * @brief EntityChart::mapTimeToPixel
 * @param time
 * @return
 */
double EntityChart::mapTimeToPixel(double time)
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
inline uint qHash(TIMELINE_DATA_KIND key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}


/**
 * @brief qHash
 * @param key
 * @param seed
 * @return
 */
inline uint qHash(LifecycleType key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
