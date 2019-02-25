#include "entitychart.h"
#include "../../../../theme.h"
#include "../../Data/Events/portlifecycleevent.h"
#include "../../Data/Events/cpuutilisationevent.h"
#include "../../Data/Events/memoryutilisationevent.h"

#include <QPainter>
#include <QPainter>
#include <algorithm>
#include <iostream>

#define BACKGROUND_OPACITY 0.25
#define LINE_WIDTH 2.0
#define POINT_BORDER 2.0
#define HIGHLIGHT_PEN_WIDTH POINT_BORDER + 1.0

#define BAR_PEN_WIDTH 1.0
#define BAR_WIDTH 5.0
#define PRINT_RENDER_TIMES false

#define BIN_PIXEL_WIDTH 20.0


/**
 * @brief EntityChart::EntityChart
 * @param experimentRunID
 * @param parent
 */
EntityChart::EntityChart(quint32 experimentRunID, QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    _experimentRunID = experimentRunID;
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
    return _experimentRunID;
}


/**
 * @brief EntityChart::addSeries
 * @param series
 */
void EntityChart::addSeries(MEDEA::EventSeries* series)
{
    if (series)
        seriesList_[series->getKind()] = series;
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
    // added 1ms on either side to include border values
    min--;
    max++;

    dataMinX_ = min;
    dataMaxX_ = max;

    displayMin_ = (dataMaxX_ - dataMinX_) * minRatio_ + dataMinX_;
    displayMax_ = (dataMaxX_ - dataMinX_) * maxRatio_ + dataMinX_;

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
 * @brief EntityChart::updateChartHeight
 * @param height
 */
void EntityChart::updateChartHeight(double height)
{
    dataHeight_ = height;
    update();
}


/**
 * @brief EntityChart::updateBinnedData
 * @param kinds
 */
void EntityChart::updateBinnedData(QSet<TIMELINE_DATA_KIND> kinds)
{
    for (auto kind : kinds) {
        updateBinnedData(kind);
    }
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

    switch (kind) {
        break;
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
        portLifecycleColor_ = defaultPortLifecycleColor_;
        break;
    case TIMELINE_DATA_KIND::WORKLOAD:
        workloadColor_ = defaultWorkloadColor_;
        break;
    case TIMELINE_DATA_KIND::CPU_UTILISATION:
        utilisationColor_ = defaultUtilisationColor_;
        break;
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
        memoryColor_ = defaultMemoryColor_;
        break;
    default: {
        // clear hovered state
        portLifecycleColor_ = defaultPortLifecycleColor_;
        workloadColor_ = defaultWorkloadColor_;
        utilisationColor_ = defaultUtilisationColor_;
        memoryColor_ = defaultMemoryColor_;
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

    portLifecycleColor_ = defaultPortLifecycleColor_;
    workloadColor_ = defaultWorkloadColor_;

    defaultUtilisationColor_ = QColor(30,144,255);
    utilisationColor_ = defaultUtilisationColor_;

    defaultMemoryColor_ = theme->getSeverityColor(Notification::Severity::SUCCESS);
    memoryColor_ = defaultMemoryColor_;

    gridColor_ = theme->getAltTextColor();
    textColor_ = theme->getTextColor();
    highlightTextColor_ = theme->getTextColor(ColorRole::SELECTED);
    backgroundColor_ = theme->getAltBackgroundColor();
    backgroundColor_.setAlphaF(BACKGROUND_OPACITY);
    highlightColor_ = theme->getHighlightColor();
    hoveredRectColor_ = theme->getActiveWidgetBorderColor();

    messagePixmap_ = theme->getImage("Icons", "exclamation", QSize(), theme->getMenuIconColor());
    updateSeriesPixmaps();
}


/**
 * @brief EntityChart::resizeEvent
 * @param event
 */
void EntityChart::resizeEvent(QResizeEvent* event)
{
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

    QColor penColor = gridColor_;
    qreal penWidth = 0.5;

    // display the y-range and send the series points that were hovered over
    if (hovered_) {
        if (containsYRange_) {
            QString minStr = QString::number(round(dataMinY_));
            QString maxStr = QString::number(round(dataMaxY_));
            int h = fontMetrics().height();
            int w = qMax(fontMetrics().width(minStr), fontMetrics().width(maxStr)) + 5;
            QRectF maxRect(width() - w, POINT_BORDER, w, h);
            QRectF minRect(width() - w, height() - h - POINT_BORDER, w, h);
            painter.setPen(textColor_);
            painter.setBrush(hoveredRectColor_);
            painter.drawRect(maxRect);
            painter.drawRect(minRect);
            painter.drawText(maxRect, maxStr, QTextOption(Qt::AlignCenter));
            painter.drawText(minRect, minStr, QTextOption(Qt::AlignCenter));
        }
        //penColor = _hoverLinePen.color();
        //penWidth = 4.0;
    }

    // NOTE: Don't use rect.bottom (rect.bottom = rect.top + rect.height - 1)
    // draw horizontal grid lines
    painter.setPen(QPen(penColor, penWidth));
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

    double barWidth = 22.0; //BAR_WIDTH;
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
    int y = rect().center().y() - barWidth / 2.0;

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
                painter.fillRect(rect, highlightColor_);
            }
            painter.drawPixmap(rect.toRect(), lifeCycleTypePixmaps_.value(event->getType()));
        } else {
            QColor color = seriesColor.darker(100 + (50 * (count - 1)));
            painter.setPen(Qt::lightGray);
            if (rectHovered(eventSeries->getKind(), rect)) {
                painter.setPen(highlightTextColor_);
                color = highlightColor_;
            }
            painter.fillRect(rect, color);
            painter.drawText(rect, QString::number(count), QTextOption(Qt::AlignCenter));
        }
    }
}


/**
 * @brief EntityChart::paintWorkloadEventSeries
 * @param painter
 */
void EntityChart::paintWorkloadEventSeries(QPainter &painter)
{
    MEDEA::EventSeries* _eventSeries = seriesList_.value(TIMELINE_DATA_KIND::WORKLOAD, 0);
    if (!_eventSeries)
        return;

    double barWidth = 22.0; //BAR_WIDTH;
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

    const auto& events = _eventSeries->getEvents();
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
    int y = rect().center().y() - barWidth / 2.0;

    for (int i = 0; i < barCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;
        QRectF rect(i * barWidth, y, barWidth, barWidth);
        if (count == 1) {
            auto event = (WorkloadEvent*) buckets[i][0];
            if (rectHovered(_eventSeries->getKind(), rect))
                painter.fillRect(rect, highlightColor_);
            painter.drawPixmap(rect.toRect(), workloadEventTypePixmaps_.value(event->getType()));
        } else {
            QColor color = seriesColor.darker(100 + (50 * (count - 1)));
            painter.setPen(Qt::lightGray);
            if (rectHovered(_eventSeries->getKind(), rect)) {
                painter.setPen(highlightTextColor_);
                color = highlightColor_;
            }
            QString countStr = count > 99 ? "99+" : QString::number(count);
            painter.fillRect(rect, color);
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

     double barWidth = 10.0; //BAR_WIDTH;
     double barCount = ceil((double)width() / barWidth);

     // because barCount needed to be rounded up, the barWidth also needs to be recalculated
     barWidth = (double) width() / barCount;

     QVector< QList<MEDEA::Event*> > buckets(barCount);
     QVector<double> bucket_endTimes;
     bucket_endTimes.reserve(barCount);

     double barTimeWidth = (displayMax_ - displayMin_) / barCount;
     double current_left = displayMin_;

     // set the bucket end times
     for (int i = 0; i < barCount; i++) {
         bucket_endTimes.append(current_left + barTimeWidth);
         current_left = bucket_endTimes.last();
     }

     const auto& events = eventSeries->getEvents();
     auto firstEvent = events.constBegin();
     auto lastEvent = events.constEnd();
     auto prevFirst = firstEvent;

     // get the first event inside the visible range
     for (; firstEvent != events.constEnd(); firstEvent++) {
         const auto& current_time = (*firstEvent)->getTimeMS();
         if (current_time > displayMin_) {
             break;
         }
         prevFirst = firstEvent;
     }

     auto current_bucket = 0;
     auto current_bucket_ittr = bucket_endTimes.constBegin();
     auto end_bucket_ittr = bucket_endTimes.constEnd();

     // put the data in the correct bucket
     for (;firstEvent != lastEvent; firstEvent++) {
         const auto& current_time = (*firstEvent)->getTimeMS();
         while (current_bucket_ittr != end_bucket_ittr) {
             if (current_time > (*current_bucket_ittr)) {
                 current_bucket_ittr ++;
                 current_bucket ++;
             } else {
                 break;
             }
         }
         if (current_bucket < barCount) {
             buckets[current_bucket].append(*firstEvent);
         }
     }

     auto max = 0.0;
     for (const auto& data : buckets) {
         for (auto e : data) {
             auto event = (CPUUtilisationEvent*) e;
             max = qMax(max, event->getUtilisation());
         }
     }
     max = 1.0;

     auto availableHeight = height() - barWidth;
     QColor seriesColor = utilisationColor_;

     painter.setPen(gridColor_);
     painter.setRenderHint(QPainter::Antialiasing, true);

     for (int i = 0; i < barCount; i++) {
         int count = buckets[i].count();
         if (count == 0)
             continue;
         auto utilisation = 0.0;
         for (auto e : buckets[i]) {
             auto event = (CPUUtilisationEvent*)e;
             utilisation += event->getUtilisation();
         }
         utilisation /= buckets[i].count();
         double y = (1 - utilisation / max) * availableHeight;
         QRectF rect(i * barWidth, y, barWidth, barWidth);
         painter.setBrush(seriesColor);
         if (rectHovered(eventSeries->getKind(), rect)) {
             painter.setBrush(highlightColor_);
         }
         painter.drawEllipse(rect);
     }

     /*if (!endPoint.isNull())
         rects.append(QRectF(endPoint.x(), (1 - endPoint.y() / max) * availableHeight, barWidth, barWidth));*/

     /*
     painter.setRenderHint(QPainter::Antialiasing, true);

     for (int i = 0; i < rects.count() - 1; i++) {
         auto rect1 = rects.at(i);
         auto rect2 = rects.at(i + 1);
         painter.setPen(QPen(seriesColor, 3.0));
         painter.drawLine(rect1.center(), rect2.center());
         painter.setPen(QPen(seriesColor, 2));
         if (rectHovered(_eventSeries->getKind(), rect1)) {
             painter.setPen(QPen(_highlightTextColor, 2.0));
             painter.setBrush(_highlightColor);
         } else {
             painter.setBrush(seriesColor);
         }
         painter.drawEllipse(rect1);
         painter.setPen(QPen(seriesColor, 2));
         if (rectHovered(_eventSeries->getKind(), rect2)) {
             painter.setPen(QPen(_highlightTextColor, 2.0));
             painter.setBrush(_highlightColor);
         } else {
             painter.setBrush(seriesColor);
         }
         painter.drawEllipse(rect2);
     }

     painter.setRenderHint(QPainter::Antialiasing, false);
     */

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

    double barWidth = 10.0; //BAR_WIDTH;
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

    auto max = 0.0;
    for (const auto& data : buckets) {
        for (auto e : data) {
            auto event = (MemoryUtilisationEvent*) e;
            max = qMax(max, event->getUtilisation());
        }
    }
    max = 1.0;

    painter.setPen(gridColor_);
    painter.setRenderHint(QPainter::Antialiasing, true);

    auto availableHeight = height() - barWidth;
    QColor seriesColor = memoryColor_;
    //QList<QRectF> rects;

    for (int i = 0; i < barCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;
        //auto event = (MemoryUtilisationEvent*) buckets[i][0];
        //auto utilisation = event->getUtilisation();
        auto utilisation = 0.0;
        for (auto e : buckets[i]) {
            auto event = (MemoryUtilisationEvent*)e;
            utilisation += event->getUtilisation();
        }
        utilisation /= buckets[i].count();
        double y = (1 - utilisation / max) * availableHeight;
        QRectF rect(i * barWidth, y, barWidth, barWidth);
        //rects.append(rect);
        painter.setBrush(seriesColor);
        if (rectHovered(eventSeries->getKind(), rect)) {
            painter.setBrush(highlightColor_);
        }
        painter.drawEllipse(rect);
    }

    /*painter.setRenderHint(QPainter::Antialiasing, true);

    for (int i = 0; i < rects.count() - 1; i++) {
        auto rect1 = rects.at(i);
        auto rect2 = rects.at(i + 1);
        painter.setPen(QPen(seriesColor, 3.0));
        painter.drawLine(rect1.center(), rect2.center());
        painter.setPen(QPen(seriesColor, 2));
        if (rectHovered(_eventSeries->getKind(), rect1)) {
            painter.setPen(QPen(_highlightTextColor, 2.0));
            painter.setBrush(_highlightColor);
        } else {
            painter.setBrush(seriesColor);
        }
        painter.drawEllipse(rect1);
        painter.setPen(QPen(seriesColor, 2));
        if (rectHovered(_eventSeries->getKind(), rect2)) {
            painter.setPen(QPen(_highlightTextColor, 2.0));
            painter.setBrush(_highlightColor);
        } else {
            painter.setBrush(seriesColor);
        }
        painter.drawEllipse(rect2);
    }*/

    painter.setRenderHint(QPainter::Antialiasing, false);
}


/**
 * @brief EntityChart::paintPortLifecycleSeries
 * @param painter
 */
void EntityChart::paintPortLifecycleSeries(QPainter &painter)
{
    int firstIndex = getBinIndexForTime(displayMin_);
    int lastIndex = getBinIndexForTime(displayMax_);

    QColor seriesColor = portLifecycleColor_;
    int y = rect().center().y() - BIN_PIXEL_WIDTH / 2.0;

    for (int i = firstIndex; i < lastIndex; i++) {
        int count = portLifecycleBinnedData_[i].count();
        if (count == 0)
            continue;
        QRectF rect(i * BIN_PIXEL_WIDTH, y, BIN_PIXEL_WIDTH, BIN_PIXEL_WIDTH);
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
}


/**
 * @brief EntityChart::rectHovered
 * @param kind
 * @param hitRect
 * @return
 */
bool EntityChart::rectHovered(TIMELINE_DATA_KIND kind, const QRectF& hitRect)
{
    auto painterRect = hitRect.adjusted(-BAR_PEN_WIDTH / 2.0, 0, BAR_PEN_WIDTH / 2.0, 0);
    if (rectHovered(painterRect)) {
        // if the hit rect is hovered, store/update the hovered time range for the provided series kind
        QPair<qint64, qint64> timeRange = {mapPixelToTime(hitRect.left()), mapPixelToTime(hitRect.right())};
        if (hoveredSeriesTimeRange_.contains(kind)) {
            timeRange.first = hoveredSeriesTimeRange_.value(kind).first;
        }
        hoveredSeriesTimeRange_[kind] = timeRange;
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
}


/**
 * @brief EntityChart::updateBinnedData
 * @param kind
 */
void EntityChart::updateBinnedData(TIMELINE_DATA_KIND kind)
{
    if (!seriesList_.contains(kind))
        return;

    // clear binned data
    auto binnedData = getBinnedData(kind);
    binnedData.clear();

    //qDebug() << "Update binned data: " << QDateTime::fromMSecsSinceEpoch(dataMinX_).toString(TIME_FORMAT) << ", " << QDateTime::fromMSecsSinceEpoch(dataMaxX_).toString(TIME_FORMAT);

    auto dataRange = dataMaxX_ - dataMinX_;
    auto binTimeWidth = dataRange / BIN_PIXEL_WIDTH;
    auto binCount = ceil(dataRange / binTimeWidth);

    QVector<double> binEndTimes(binCount);
    //binEndTimes.reserve(binCount);

    // calculate the bin end times
    auto currentTime = displayMin_;
    for (auto i = 0; i < binCount; i++) {
        binEndTimes.append(currentTime + binTimeWidth);
        currentTime = binEndTimes.last();
    }

    for (auto series : seriesList_.values(kind)) {
        if (!series)
            continue;

        //QVector< QList<MEDEA::Event*> > binnedData(binCount);

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
            if (currentBin < binCount) {
                binnedData[currentBin].append(*eventItr);
            }
        }

        //binnedData_[series->getKind()] = binnedData;
    }
}


/**
 * @brief EntityChart::updateSeriesPixmaps
 */
void EntityChart::updateSeriesPixmaps()
{
    Theme* theme = Theme::theme();
    QColor pixmapColor = theme->getBackgroundColor();

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
        lifeCycleTypePixmaps_[LifecycleType::NO_TYPE] = theme->getImage("Icons", "circleQuestion", QSize(), pixmapColor);
        lifeCycleTypePixmaps_[LifecycleType::CONFIGURE] = theme->getImage("Icons", "gear", QSize(), pixmapColor);
        lifeCycleTypePixmaps_[LifecycleType::ACTIVATE] = theme->getImage("Icons", "clockDark", QSize(), pixmapColor);
        lifeCycleTypePixmaps_[LifecycleType::PASSIVATE] = theme->getImage("Icons", "circleMinusDark", QSize(), pixmapColor);
        lifeCycleTypePixmaps_[LifecycleType::TERMINATE] = theme->getImage("Icons", "circleRadio", QSize(), pixmapColor);
    }

    if (colorWorkerPixmaps) {
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::STARTED] = theme->getImage("Icons", "play", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::FINISHED] = theme->getImage("Icons", "avStop", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleMessage", QSize(), QColor(72, 151, 189));
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::WARNING] = theme->getImage("Icons", "triangleCritical", QSize(), theme->getSeverityColor(Notification::Severity::WARNING));
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::ERROR_EVENT] = theme->getImage("Icons", "circleCrossDark", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
    } else {
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::STARTED] = theme->getImage("Icons", "play", QSize(), pixmapColor);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::FINISHED] = theme->getImage("Icons", "avStop", QSize(), pixmapColor);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleMessage", QSize(), pixmapColor);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::WARNING] = theme->getImage("Icons", "triangleCritical", QSize(), pixmapColor);
        workloadEventTypePixmaps_[WorkloadEvent::WorkloadEventType::ERROR_EVENT] = theme->getImage("Icons", "circleCrossDark", QSize(), pixmapColor);
    }
}


/**
 * @brief EntityChart::getBinIndexForTime
 * @param time
 * @return
 */
int EntityChart::getBinIndexForTime(double time)
{
    auto dataRange = dataMaxX_ - dataMinX_;
    auto binTimeWidth = dataRange / BIN_PIXEL_WIDTH;
    auto index = floor((time - displayMin_) / binTimeWidth);

    /*
    if (time == displayMin_) {
        index = floor(index);
    } else if (time == displayMax_) {
        index = ceil(index);
    }
    */

    return index;
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
    default:
        return QVector<QList<MEDEA::Event*>>();
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
