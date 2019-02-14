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
    _hoveredSeriesKind = TIMELINE_DATA_KIND::DATA;

    _dataMinX = DBL_MAX;
    _dataMaxX = DBL_MIN;
    _dataMinY = DBL_MAX;
    _dataMaxY = DBL_MIN;

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
 * @brief EntityChart::addEventSeries
 * @param series
 */
void EntityChart::addEventSeries(MEDEA::EventSeries* series)
{
    if (series)
        _seriesList[series->getKind()] = series;
}


/**
 * @brief EntityChart::removeEventSeries
 * @param kind
 */
void EntityChart::removeEventSeries(TIMELINE_DATA_KIND kind)
{
    _seriesList.remove(kind);
}


/**
 * @brief EntityChart::getSeries
 * @return
 */
const QHash<TIMELINE_DATA_KIND, MEDEA::EventSeries*>& EntityChart::getSeries()
{
    return _seriesList;
}


/**
 * @brief EntityChart::getHovereSeriesKinds
 * @return
 */
const QList<TIMELINE_DATA_KIND> EntityChart::getHovereSeriesKinds()
{
    return _hoveredSeriesTimeRange.keys();
}


/**
 * @brief EntityChart::getHoveredTimeRange
 * @param kind
 * @return
 */
const QPair<qint64, qint64> EntityChart::getHoveredTimeRange(TIMELINE_DATA_KIND kind)
{
    return _hoveredSeriesTimeRange.value(kind, {-1, -1});
}


/**
 * @brief EntityChart::setDisplayMinRatio
 * @param ratio
 */
void EntityChart::setDisplayMinRatio(double ratio)
{
    _minRatio = ratio;
    _displayMin = (_dataMaxX - _dataMinX) * ratio + _dataMinX;
    update();
}


/**
 * @brief EntityChart::setDisplayMaxRatio
 * @param ratio
 */
void EntityChart::setDisplayMaxRatio(double ratio)
{
    _maxRatio = ratio;
    _displayMax = (_dataMaxX - _dataMinX) * ratio + _dataMinX;
    update();
}


/**
 * @brief EntityChart::setDisplayRangeRatio
 * @param minRatio
 * @param maxRatio
 */
void EntityChart::setDisplayRangeRatio(double minRatio, double maxRatio)
{
    _minRatio = minRatio;
    _maxRatio = maxRatio;
    _displayMin = (_dataMaxX - _dataMinX) * minRatio + _dataMinX;
    _displayMax = (_dataMaxX - _dataMinX) * maxRatio + _dataMinX;
    update();
}


/**
 * @brief EntityChart::isHovered
 * @return
 */
bool EntityChart::isHovered()
{
    return _hovered;
}


/**
 * @brief EntityChart::setHovered
 * @param visible
 */
void EntityChart::setHovered(bool visible)
{
    _hovered = visible;
}


/**
 * @brief EntityChart::setHoveredRect
 * @param rect
 */
void EntityChart::setHoveredRect(QRectF rect)
{
    if (rect != _hoveredRect) {
        QPoint pos = mapFromParent(rect.topLeft().toPoint());
        rect.moveTo(pos.x(), 0);
        _hoveredRect = rect;
    }
}


/**
 * @brief EntityChart::setSeriesKindVisible
 * @param kind
 * @param visible
 */
void EntityChart::setSeriesKindVisible(TIMELINE_DATA_KIND kind, bool visible)
{
    _seriesKindVisible[kind] = visible;
    update();
}


/**
 * @brief EntityChart::seriesKindHovered
 * @param kind
 */
void EntityChart::seriesKindHovered(TIMELINE_DATA_KIND kind)
{
    if (kind == _hoveredSeriesKind)
        return;

    _portLifecycleColor = _backgroundColor;
    _workloadColor = _backgroundColor;
    _utilisationColor = _backgroundColor;
    _memoryColor = _backgroundColor;

    switch (kind) {
        break;
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
        _portLifecycleColor = _defaultPortLifecycleColor;
        break;
    case TIMELINE_DATA_KIND::WORKLOAD:
        _workloadColor = _defaultWorkloadColor;
        break;
    case TIMELINE_DATA_KIND::CPU_UTILISATION:
        _utilisationColor = _defaultUtilisationColor;
        break;
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
        _memoryColor = _defaultMemoryColor;
        break;
    default: {
        // clear hovered state
        _portLifecycleColor = _defaultPortLifecycleColor;
        _workloadColor = _defaultWorkloadColor;
        _utilisationColor = _defaultUtilisationColor;
        _memoryColor = _defaultMemoryColor;
        break;
    }
    }

    _hoveredSeriesKind = kind;
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

    _portLifecycleColor = _defaultPortLifecycleColor;
    _workloadColor = _defaultWorkloadColor;

    _defaultUtilisationColor = QColor(30,144,255);
    _utilisationColor = _defaultUtilisationColor;

    _defaultMemoryColor = theme->getSeverityColor(Notification::Severity::SUCCESS);
    _memoryColor = _defaultMemoryColor;

    _backgroundColor = theme->getAltBackgroundColor();
    _backgroundColor.setAlphaF(BACKGROUND_OPACITY);
    _highlightColor = theme->getHighlightColor();
    _highlightTextColor = theme->getTextColor(ColorRole::SELECTED);
    _hoveredRectColor = theme->getActiveWidgetBorderColor();

    _gridPen = QPen(theme->getAltTextColor(), 0.5);
    _hoverLinePen = QPen(theme->getTextColor(), POINT_BORDER, Qt::PenStyle::DotLine);

    _messagePixmap = theme->getImage("Icons", "exclamation", QSize(), theme->getMenuIconColor());
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
        if (kind != _hoveredSeriesKind)
            paintSeries(painter, kind);
    }
    paintSeries(painter, _hoveredSeriesKind);

    QColor penColor = _gridPen.color();
    qreal penWidth = _gridPen.widthF();

    // display the y-range and send the series points that were hovered over
    if (_hovered) {
        if (_containsYRange) {
            QString minStr = QString::number(round(_dataMinY));
            QString maxStr = QString::number(round(_dataMaxY));
            int h = fontMetrics().height();
            int w = qMax(fontMetrics().width(minStr), fontMetrics().width(maxStr)) + 5;
            QRectF maxRect(width() - w, POINT_BORDER, w, h);
            QRectF minRect(width() - w, height() - h - POINT_BORDER, w, h);
            painter.setPen(_hoverLinePen.color());
            painter.setBrush(_hoveredRectColor);
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
    if (!_seriesKindVisible.value(kind, false))
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
    MEDEA::EventSeries* eventSeries = _seriesList.value(TIMELINE_DATA_KIND::PORT_LIFECYCLE, 0);
    if (!eventSeries)
        return;

    double barWidth = 22.0; //BAR_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<MEDEA::Event*> > buckets(barCount);
    QVector<double> bucket_endTimes;
    bucket_endTimes.reserve(barCount);

    double barTimeWidth = (_displayMax - _displayMin) / barCount;
    double current_left = _displayMin;
    for (int i = 0; i < barCount; i++) {
        bucket_endTimes.append(current_left + barTimeWidth);
        current_left = bucket_endTimes.last();
    }

    const auto& events = eventSeries->getEvents();
    auto current = events.constBegin();
    auto upper = events.constEnd();
    for (; current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        if (current_time > _displayMin) {
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

    QColor seriesColor = _portLifecycleColor;
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
                _hoveredSeriesTimeRange[eventSeries->getKind()] = {event->getTimeMS(), event->getTimeMS()};
                painter.fillRect(rect, _highlightColor);
            }
            painter.drawPixmap(rect.toRect(), _lifeCycleTypePixmaps.value(event->getType()));
        } else {
            QColor color = seriesColor.darker(100 + (50 * (count - 1)));
            painter.setPen(Qt::lightGray);
            if (rectHovered(eventSeries->getKind(), rect)) {
                painter.setPen(_highlightTextColor);
                color = _highlightColor;
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
    MEDEA::EventSeries* _eventSeries = _seriesList.value(TIMELINE_DATA_KIND::WORKLOAD, 0);
    if (!_eventSeries)
        return;

    double barWidth = 22.0; //BAR_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<MEDEA::Event*> > buckets(barCount);
    QVector<double> bucket_endTimes;
    bucket_endTimes.reserve(barCount);

    double barTimeWidth = (_displayMax - _displayMin) / barCount;
    double current_left = _displayMin;
    for (int i = 0; i < barCount; i++) {
        bucket_endTimes.append(current_left + barTimeWidth);
        current_left = bucket_endTimes.last();
    }

    const auto& events = _eventSeries->getEvents();
    auto current = events.constBegin();
    auto upper = events.constEnd();
    for (; current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        if (current_time > _displayMin) {
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

    QColor seriesColor = _workloadColor;
    int y = rect().center().y() - barWidth / 2.0;

    for (int i = 0; i < barCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;
        QRectF rect(i * barWidth, y, barWidth, barWidth);
        if (count == 1) {
            auto event = (WorkloadEvent*) buckets[i][0];
            if (rectHovered(_eventSeries->getKind(), rect))
                painter.fillRect(rect, _highlightColor);
            painter.drawPixmap(rect.toRect(), _workloadEventTypePixmaps.value(event->getType()));
        } else {
            QColor color = seriesColor.darker(100 + (50 * (count - 1)));
            painter.setPen(Qt::lightGray);
            if (rectHovered(_eventSeries->getKind(), rect)) {
                painter.setPen(_highlightTextColor);
                color = _highlightColor;
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
    MEDEA::EventSeries* eventSeries = _seriesList.value(TIMELINE_DATA_KIND::CPU_UTILISATION, 0);
    if (!eventSeries)
         return;

     double barWidth = 10.0; //BAR_WIDTH;
     double barCount = ceil((double)width() / barWidth);

     // because barCount needed to be rounded up, the barWidth also needs to be recalculated
     barWidth = (double) width() / barCount;

     QVector< QList<MEDEA::Event*> > buckets(barCount);
     QVector<double> bucket_endTimes;
     bucket_endTimes.reserve(barCount);

     double barTimeWidth = (_displayMax - _displayMin) / barCount;
     double current_left = _displayMin;

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
         if (current_time > _displayMin) {
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
     QColor seriesColor = _utilisationColor;

     painter.setPen(_gridPen.color());
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
             painter.setBrush(_highlightColor);
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
    MEDEA::EventSeries* eventSeries = _seriesList.value(TIMELINE_DATA_KIND::MEMORY_UTILISATION, 0);
    if (!eventSeries)
        return;

    double barWidth = 10.0; //BAR_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<MEDEA::Event*> > buckets(barCount);
    QVector<double> bucket_endTimes;
    bucket_endTimes.reserve(barCount);

    double barTimeWidth = (_displayMax - _displayMin) / barCount;
    double current_left = _displayMin;
    for (int i = 0; i < barCount; i++) {
        bucket_endTimes.append(current_left + barTimeWidth);
        current_left = bucket_endTimes.last();
    }

    const auto& events = eventSeries->getEvents();
    auto current = events.constBegin();
    auto upper = events.constEnd();
    for (; current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        if (current_time > _displayMin) {
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

    painter.setPen(_gridPen.color());
    painter.setRenderHint(QPainter::Antialiasing, true);

    auto availableHeight = height() - barWidth;
    QColor seriesColor = _memoryColor;
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
            painter.setBrush(_highlightColor);
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
 * @brief EntityChart::paintBarSeries
 * @param painter
 * @param points
 */
void EntityChart::paintBarSeries(QPainter &painter)
{
    /*
    auto start = QDateTime::currentMSecsSinceEpoch();

    MEDEA::BarSeries* series = (MEDEA::BarSeries*)_seriesList.value(TIMELINE_DATA_KIND::BAR, 0);
    if (!series)
        return;

    if (!_seriesKindVisible.value(TIMELINE_DATA_KIND::LINE, false))
        return;

    const auto& data = series->getConstData2();
    auto current = data.lowerBound(_displayMin);
    auto upper = data.upperBound(_displayMax);

    double barWidth = BAR_WIDTH;
    int barCount = ceil(width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    // bar/bucket count
    QVector< QList<MEDEA::BarData*> > buckets(barCount);
    QList<double> bucketEndtimes;
    double barTimeWidth = (_displayMax - _displayMin) / barCount;
    
    auto currentLeft = _displayMin;
    for (int i = 0; i < barCount; i++) {
        bucketEndtimes.append(currentLeft + barTimeWidth);
        currentLeft = bucketEndtimes.last();
    }

    auto currentBucket = 0;
    auto currentBucketIttr = bucketEndtimes.constBegin();
    auto endBucketIttr = bucketEndtimes.constEnd();

    // put the data in the correct bucket
    for (;current != upper; current++) {
        const auto& currentX = current.key();
        while (currentBucketIttr != endBucketIttr) {
            if (currentX > (*currentBucketIttr)) {
                currentBucketIttr++;
                currentBucket++;
            } else {
                break;
            }
        }
        if (currentBucket < barCount)
            buckets[currentBucket] += current.value();
    }

    QColor seriesColor = _lineColor;
    painter.setPen(QPen(seriesColor.lighter(_borderColorDelta), BAR_PEN_WIDTH));

    for (int i = 0; i < barCount; i++) {
        auto size = buckets[i].size();
        if (size > 0) {
            auto dataMin = DBL_MAX;
            auto dataMax = 0.0;
            for (const auto& data : buckets[i]) {
                dataMin = qMin(dataMin, data->getMin());
                dataMax = qMax(dataMax, data->getMax());
            }
            auto dataToPixel = height() / _dataMaxY;
            auto pixelMin = dataMin * dataToPixel;
            auto pixelMax = dataMax * dataToPixel;
            auto barHeight = pixelMax - pixelMin;
            auto color = seriesColor.darker(100 * (1.0 + size / dataMax));
            auto rect = QRectF(i * barWidth, height() - pixelMax, barWidth, barHeight);

            if (rectHovered(TIMELINE_DATA_KIND::BAR, rect)) {
                color.setHsv(qAbs(color.hue() - 180), 255, 255);
            }
            painter.setBrush(color);

            if (size == 1) {
                const auto& data = buckets[i][0];
                paintBarData(painter, rect, color, data->getData());
                painter.setBrush(Qt::NoBrush);
            }
            painter.drawRect(rect);
        }
    }

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "BarSeries Render Took: " << finish - start << "MS. Returned: " << data.count();
        */
}


/**
 * @brief EntityChart::paintBarData
 * @param painter
 * @param barRect
 * @param color
 * @param data
 */
void EntityChart::paintBarData(QPainter &painter, const QRectF &barRect, const QColor &color, const QVector<double> &data)
{
    if (data.isEmpty() || data.count() > 5)
        return;

    double dataToPixel = height() / _dataMaxY;
    painter.fillRect(barRect, color);

    switch (data.count()) {
    case 5: {
        auto barHeight = (data[1] - data[2]) * dataToPixel;
        auto upperRect = QRectF(barRect.x(), height() - data[1] * dataToPixel, BAR_WIDTH, barHeight);
        barHeight = (data[2] - data[3]) * dataToPixel;
        auto lowerRect = QRectF(barRect.x(), height() - data[2] * dataToPixel, BAR_WIDTH, barHeight);
        painter.fillRect(upperRect, color.lighter());
        painter.fillRect(lowerRect, color.darker());
    }
    case 3: {
        auto midRect = QRectF(barRect.x(), height() - data[1] * dataToPixel - 1, BAR_WIDTH, 2);
        painter.fillRect(midRect, color);
    }
    default:
        break;
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
        if (_hoveredSeriesTimeRange.contains(kind)) {
            timeRange.first = _hoveredSeriesTimeRange.value(kind).first;
        }
        _hoveredSeriesTimeRange[kind] = timeRange;
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
    return _hoveredRect.intersects(hitRect);
}


/**
 * @brief EntityChart::clearHoveredLists
 */
void EntityChart::clearHoveredLists()
{
    // clear previously hovered series kinds
    _hoveredSeriesTimeRange.clear();
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

    _dataMinX = min;
    _dataMaxX = max;

    _displayMin = (_dataMaxX - _dataMinX) * _minRatio + _dataMinX;
    _displayMax = (_dataMaxX - _dataMinX) * _maxRatio + _dataMinX;

    update();
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

    switch (_hoveredSeriesKind) {
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
        _lifeCycleTypePixmaps[LifecycleType::NO_TYPE] = theme->getImage("Icons", "circleQuestion", QSize(), theme->getAltTextColor());
        _lifeCycleTypePixmaps[LifecycleType::CONFIGURE] = theme->getImage("Icons", "gear", QSize(), theme->getSeverityColor(Notification::Severity::WARNING));
        _lifeCycleTypePixmaps[LifecycleType::ACTIVATE] = theme->getImage("Icons", "clockDark", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
        _lifeCycleTypePixmaps[LifecycleType::PASSIVATE] = theme->getImage("Icons", "circleMinusDark", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
        _lifeCycleTypePixmaps[LifecycleType::TERMINATE] = theme->getImage("Icons", "circleRadio", QSize(), theme->getMenuIconColor());
    } else {
        _lifeCycleTypePixmaps[LifecycleType::NO_TYPE] = theme->getImage("Icons", "circleQuestion", QSize(), pixmapColor);
        _lifeCycleTypePixmaps[LifecycleType::CONFIGURE] = theme->getImage("Icons", "gear", QSize(), pixmapColor);
        _lifeCycleTypePixmaps[LifecycleType::ACTIVATE] = theme->getImage("Icons", "clockDark", QSize(), pixmapColor);
        _lifeCycleTypePixmaps[LifecycleType::PASSIVATE] = theme->getImage("Icons", "circleMinusDark", QSize(), pixmapColor);
        _lifeCycleTypePixmaps[LifecycleType::TERMINATE] = theme->getImage("Icons", "circleRadio", QSize(), pixmapColor);
    }

    if (colorWorkerPixmaps) {
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::STARTED] = theme->getImage("Icons", "play", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS));
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::FINISHED] = theme->getImage("Icons", "avStop", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleMessage", QSize(), QColor(72, 151, 189));
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::WARNING] = theme->getImage("Icons", "triangleCritical", QSize(), theme->getSeverityColor(Notification::Severity::WARNING));
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::ERROR_EVENT] = theme->getImage("Icons", "circleCrossDark", QSize(), theme->getSeverityColor(Notification::Severity::ERROR));
    } else {
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::STARTED] = theme->getImage("Icons", "play", QSize(), pixmapColor);
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::FINISHED] = theme->getImage("Icons", "avStop", QSize(), pixmapColor);
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::MESSAGE] = theme->getImage("Icons", "speechBubbleMessage", QSize(), pixmapColor);
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::WARNING] = theme->getImage("Icons", "triangleCritical", QSize(), pixmapColor);
        _workloadEventTypePixmaps[WorkloadEvent::WorkloadEventType::ERROR_EVENT] = theme->getImage("Icons", "circleCrossDark", QSize(), pixmapColor);
    }
}


/**
 * @brief EntityChart::mapPixelToTime
 * @param x
 * @return
 */
qint64 EntityChart::mapPixelToTime(double x)
{
    auto timeRange = _displayMax - _displayMin;
    auto ratio = x / width();
    return _displayMin + timeRange * ratio;
}


/**
 * @brief EntityChart::mapTimeToPixel
 * @param time
 * @return
 */
double EntityChart::mapTimeToPixel(double time)
{
    auto timeRange = _displayMax - _displayMin;
    auto adjustedTime = time - _displayMin;
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
